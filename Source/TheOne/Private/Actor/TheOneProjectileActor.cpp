// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/TheOneProjectileActor.h"
#include "NiagaraSystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATheOneProjectileActor::ATheOneProjectileActor()
{
 	PrimaryActorTick.bCanEverTick = false;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	Sphere->SetCollisionProfileName("Projectile");

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(Sphere);

	MainEffectComponent = CreateDefaultSubobject<UNiagaraComponent>("MainEffectComponent");
	MainEffectComponent->SetupAttachment(Sphere);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

UAbilitySystemComponent* ATheOneProjectileActor::GetAbilitySystemComponent() const
{
	if (!SourceActor.IsValid())
	{
		return nullptr;
	}

	if (auto Character = Cast<ATheOneCharacterBase>(SourceActor.Get()))
	{
		return Character->GetAbilitySystemComponent();
	} 

	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor.Get());
}

ATheOneCharacterBase* ATheOneProjectileActor::GetSourceCharacter() const
{
	if (!SourceActor.IsValid())
	{
		return nullptr;
	}

	if (auto Character = Cast<ATheOneCharacterBase>(SourceActor.Get()))
	{
		return Character;
	}

	const ITheOneCharacterSpawnActor* ISource = Cast<ITheOneCharacterSpawnActor>(SourceActor.Get());
	if (ISource)
	{
		return ISource->GetSourceCharacter();
	}

	return nullptr;
}

AActor* ATheOneProjectileActor::GetHitTarget() const
{
	return TargetActor.Get();
}

// Called when the game starts or when spawned
void ATheOneProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATheOneProjectileActor::Tick(float DeltaSeconds)
{
	// C++ Tick
	if (TargetActor.IsValid())
	{
		// 计算是否达到目标位置
		if (FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation() ) < 15.f)
		{
			// 到达目标位置
			ProjectileMovementComponent->Velocity = FVector::ZeroVector;
			OnHitTarget();
			PrimaryActorTick.bCanEverTick = false;
			// 销毁自身
			Destroy();
		} else
		{
			// 计算朝向目标的方向
			FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// 更新速度方向
			ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->MaxSpeed;
		}
	}

	// 蓝图Tick
	Super::Tick(DeltaSeconds);
}

void ATheOneProjectileActor::SetUp(const FTheOneProjectileConfig* InConfigRow, AActor* InSourceActor,
	UTheOneGameplayAbility* InAbility, AActor* InTargetActor, const FVector& InDstLocation,
	ETheOneProjectileFlyType InFlyType, const float InSpeed, const TArray<FTheOneAbilityActionData*>& InHitActionData)
{
	SourceActor = InSourceActor;
	SourceAbility = InAbility;
	TargetActor = InTargetActor;
	DstLocation = InDstLocation;

	ProjectileMovementComponent->InitialSpeed = InSpeed;
	ProjectileMovementComponent->MaxSpeed = InSpeed;
	
	if (InConfigRow)
	{
		// Todo: 特效资源需要进行预加载，否则一段时间内会在编辑材质，导致特效无法显示
		UNiagaraSystem* MainE = InConfigRow->MainEffect.LoadSynchronous();
		MainEffectComponent->SetAsset(MainE);
		// Todo: 根据配置是否是抛物线
		// Todo: 在Tick里调整朝向，始终朝向InDstLocation
	}

	switch (InFlyType) {
		case ETheOneProjectileFlyType::Invalid:
			break;
		case ETheOneProjectileFlyType::Trace:
			PrimaryActorTick.bCanEverTick = true;
		break;
		case ETheOneProjectileFlyType::FixedLocation:
			break;
		case ETheOneProjectileFlyType::Lob:
			break;
		case ETheOneProjectileFlyType::Line:
			break;
		case ETheOneProjectileFlyType::Instant:
			break;
	}

	FTheOneAbilityTargetSelector InterFaceSelector;
	InterFaceSelector.TargetSelectorType = ETheOneTargetSelectorType::FromInterface;
	HitActionContainer.TargetSelector = InterFaceSelector;

	HitActionContainer.Actions.Empty();
	
	for (auto& HitAction : InHitActionData)
	{
		FTheOneAbilityAction RTAction;
		RTAction.ActionData = *HitAction;
		HitActionContainer.Actions.Add(RTAction);
	}
}

void ATheOneProjectileActor::OnHitTarget()
{
	auto Level = SourceAbility.IsValid()? SourceAbility->GetLevel() : 1;
	
	for (auto& HitAction : HitActionContainer.Actions)
	{
		HitAction.DoAction(this, HitActionContainer.TargetSelector, Level, SourceAbility.Get());
	}
}
