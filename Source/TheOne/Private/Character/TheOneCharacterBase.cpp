// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TheOneCharacterBase.h"

#include "AIController.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/TheOneCharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/TheOneEventSystem.h"
#include "HexGrid/HexPathFollowingComponent.h"

// 使用TheOneCharacterMovementComponent
ATheOneCharacterBase::ATheOneCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTheOneCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HeadBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HeadBarWidgetComponent");
	HeadBarWidgetComponent->SetupAttachment(RootComponent);

	AbilitySystemComponent = CreateDefaultSubobject<UTheOneAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(false);

	TheOneAbilitySystemComponent = Cast<UTheOneAbilitySystemComponent>(AbilitySystemComponent);
	
	AttributeSet = CreateDefaultSubobject<UTheOneAttributeSet>("AttributeSet");
	LifeAttributeSet = CreateDefaultSubobject<UTheOneLifeAttributeSet>("LifeAttributeSet");
}

void ATheOneCharacterBase::SpawnInit()
{
	GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
	GetCharacterMovement()->RotationRate = FRotator(0.f, AttributeSet->GetTurnSpeed(), 0.f);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &ATheOneCharacterBase::OnMoveSpeedChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetTurnSpeedAttribute()).AddUObject(this, &ATheOneCharacterBase::OnRotationRateChanged);
}

const FTheOneCharacterConfig& ATheOneCharacterBase::GetConfig() const
{
	auto Config = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable->FindRow<FTheOneCharacterConfig>(ConfigRowName, "ATheOneCharacterBase::GetConfig");
	if (Config)
	{
		return *Config;
	}

	static FTheOneCharacterConfig EmptyConfig;
	return EmptyConfig;
}

const FTheOneAbilityCache* ATheOneCharacterBase::GetAbilityCacheByIntPayload(int32 InIntPayload)
{
	int Count = 0;
	for (const auto& Ability : AbilityCaches)
	{
		for (const auto& AbilityData : Ability.Value)
		{
			if (Count == InIntPayload)
			{
				return &AbilityData;
			}
			Count++;
		}
	}
	
	return nullptr;
}

void ATheOneCharacterBase::BeforeEnterBattle()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterGetTurn.AddUObject(this, &ATheOneCharacterBase::OnGetTurn);
	EventSystem->OnCharacterEndTurn.AddUObject(this, &ATheOneCharacterBase::OnEndTurn);
}

void ATheOneCharacterBase::AfterEndBattle()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterGetTurn.RemoveAll(this);
	EventSystem->OnCharacterEndTurn.RemoveAll(this);
}

UTheOneAttackGA* ATheOneCharacterBase::DoAttack_Implementation(ETheOneTryActiveResult& Result)
{
	// TheOneAbilitySystemComponent->TryTheOneActivateAbility(AttackAbilitySpecHandle, Result);
	// switch (Result) {
	// 	case ETheOneTryActiveResult::Succeed:
	// 		break;
	// 	case ETheOneTryActiveResult::SucceedNoInstanced:
	// 		break;
	// 	case ETheOneTryActiveResult::Failed:
	// 		UE_LOG(LogTheOne, Warning, TEXT("ATheOneCharacterBase::DoAttack_Implementation, Failed to activate ability"));
	// 	break;
	// }

	return nullptr;
}

UTheOneGeneralGA* ATheOneCharacterBase::DoAbility_Implementation(ETheOneUseAbilityCommandType InUseAbilityCommandType,
	int32 InIntPayload, AActor* InTargetActor, const FVector& InLocation, ETheOneTryActiveResult& Result)
{
	FGameplayAbilitySpecHandle ToReleaseAbilitySpecHandle;
	UTheOneGeneralGA* RetGA = nullptr;
	switch (InUseAbilityCommandType) {
		case ETheOneUseAbilityCommandType::Invalid:
			Result = ETheOneTryActiveResult::Failed;
			return nullptr;
		case ETheOneUseAbilityCommandType::UseItem:
			// Todo:
			break;
		case ETheOneUseAbilityCommandType::UseAbility:
			// Todo: 根据Index执行技能
			{
				const auto AbilityCache = GetAbilityCacheByIntPayload(InIntPayload);
				check(AbilityCache)
				ToReleaseAbilitySpecHandle = AbilityCache->AbilitySpecHandle;
				RetGA = AbilityCache->AbilityGA.Get();
			}
			break;
	}
	
	TheOneAbilitySystemComponent->TryTheOneActivateAbility(ToReleaseAbilitySpecHandle, Result);
	switch (Result) {
		case ETheOneTryActiveResult::Succeed:
			break;
		case ETheOneTryActiveResult::SucceedNoInstanced:
			break;
		case ETheOneTryActiveResult::Failed:
			UE_LOG(LogTheOne, Warning, TEXT("ATheOneCharacterBase::DoAbility_Implementation, Failed to activate ability"));
		break;
	}

	return RetGA;
}

bool ATheOneCharacterBase::IsDead_Implementation() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TheOneGameplayTags::Status_Death);
}

bool ATheOneCharacterBase::IsStun_Implementation() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TheOneGameplayTags::Status_Stun);
}

AActor* ATheOneCharacterBase::GetTargetActor_Implementation() const
{
	auto AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return nullptr;
	}
	return Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
}

void ATheOneCharacterBase::Die()
{
	// 标记死亡
	AbilitySystemComponent->AddLooseGameplayTag(TheOneGameplayTags::Status_Death);
	
	auto AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		auto HexPathFollowingComponent = Cast<UHexPathFollowingComponent>(AIController->GetPathFollowingComponent());
		if (HexPathFollowingComponent)
		{
			HexPathFollowingComponent->OnAIDead();
		}
	}

	// 关闭碰撞
	this->SetActorEnableCollision(false);
	// 蓝图中等待一段时间隐藏血条
	
	BP_Die();

	OnceOnDead.Broadcast();
	OnceOnDead.Clear();
}

void ATheOneCharacterBase::OnSelected()
{
	BP_OnSelected();
}

void ATheOneCharacterBase::OnDeselected()
{
	BP_OnDeselected();
}

const FHCubeCoord& ATheOneCharacterBase::GetCurrentHexCoord() const
{
	return CurrentCoord;
}

const FHCubeCoord& ATheOneCharacterBase::GetBookedHexCoord() const
{
	return BookedCoord;
}

void ATheOneCharacterBase::SetCurrentHexCoord(const FHCubeCoord& InCoord)
{
	CurrentCoord = InCoord;
	OnEnterNewCoord.Broadcast(InCoord);
}

void ATheOneCharacterBase::SetBookedHexCoord(const FHCubeCoord& InCoord)
{
	BookedCoord = InCoord;
}

uint32 ATheOneCharacterBase::GetFlag() const
{
	auto Ctrl = GetController();
	if (Ctrl)
	{
		return Ctrl->GetUniqueID();
	}
	UE_LOG(LogTheOne, Error, TEXT("ATheOneCharacterBase::GetControllerUniqueID, %s Controller is nullptr, "), *GetName());
	return 0;
}

// Called when the game starts or when spawned
void ATheOneCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATheOneCharacterBase::BeginDestroy()
{
	Super::BeginDestroy();
}

void ATheOneCharacterBase::OnMoveSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	GetCharacterMovement()->MaxWalkSpeed = OnAttributeChangeData.NewValue;
}

void ATheOneCharacterBase::OnRotationRateChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	GetCharacterMovement()->RotationRate = FRotator(0.f, OnAttributeChangeData.NewValue, 0.f);
}

void ATheOneCharacterBase::OnGetTurn(ATheOneCharacterBase* InCharacter)
{
	if (InCharacter == this)
	{
		BP_OnGetTurn();
	}
}

void ATheOneCharacterBase::OnEndTurn(ATheOneCharacterBase* InCharacter)
{
	if (InCharacter == this)
	{
		BP_OnEndTurn();
	}
}
