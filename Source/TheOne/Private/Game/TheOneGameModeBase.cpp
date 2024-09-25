// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneGameModeBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AbilitySystem/Abilities/TheOneDataDrivePassiveGA.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "Actor/TheOneInteractActor.h"
#include "AI/TheOneAIController.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Development/TheOneGeneralSettings.h"
#include "Game/GameModeComponents/TheOneAutoChessBattleComponent.h"
#include "Game/GameStateComponets/TheOneSpawnManagerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "HexGrid/HexAStarNavMesh.h"
#include "HexGrid/HexGrid.h"
#include "Item/TheOneItemSystem.h"
#include "Subsystems/TheOneContextSystem.h"

UTheOneAutoChessBattleComponent* ATheOneGameModeBase::GetAutoChessBattleComponent() const
{
	if (AutoChessBattleComponentCache.IsValid())
	{
		return AutoChessBattleComponentCache.Get();
	}

	return nullptr;
}

void ATheOneGameModeBase::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);
	OnTheOneGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

UClass* ATheOneGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (auto TheOneAIController = Cast<ATheOneAIController>(InController))
	{
		auto DT = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable.LoadSynchronous();
		auto Row = TheOneAIController->GetPawnSpawnInfo().CharacterTemplateRowName;
		auto Template = DT->FindRow<FTheOneCharacterConfig>(Row, "ATheOneGameModeBase::GetDefaultPawnClassForController_Implementation");

		if (Template != nullptr)
		{
			return Template->Custom.MinionClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool ATheOneGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

bool ATheOneGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal,
	FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

AActor* ATheOneGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (UTheOneSpawnManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UTheOneSpawnManagerComponent>())
	{
		auto StartPos = PlayerSpawningComponent->ChoosePlayerStart(Player);
		if (StartPos)
		{
			return StartPos;
		}

		UE_LOG(LogTheOne, Error, TEXT("Failed to find a player start for player %s"), *Player->GetName());
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATheOneGameModeBase::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	UE_LOG(LogTheOne, Error, TEXT("Failed to restart player %s, Todo: 检查为什么会失败，以及再次重试"), *NewPlayer->GetName());
}


void ATheOneGameModeBase::SpawnOneInteractActor(const TSubclassOf<ATheOneInteractActor> InteractActorClass,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	// 仅蓝图类支持
	SpawnInfo.bDeferConstruction = true;
	
	ATheOneInteractActor* NewInteractActor = GetWorld()->SpawnActor<ATheOneInteractActor>(
		InteractActorClass, SpawnTransform, SpawnInfo);
	if (NewInteractActor != nullptr)
	{
		NewInteractActor->FinishSpawning(SpawnTransform);
		NewInteractActor->SetActorLocation(SpawnTransform.GetLocation() + NewInteractActor->SpawnOffset.GetLocation());
		NewInteractActor->SetActorRotation(SpawnTransform.GetRotation() * NewInteractActor->SpawnOffset.GetRotation());
		NewInteractActor->SetActorScale3D(SpawnTransform.GetScale3D() * NewInteractActor->SpawnOffset.GetScale3D());
	}
	else
	{
		UE_LOG(LogTheOne, Error, TEXT("Failed to spawn InteractActor %s"), *InteractActorClass->GetName());
	}
}

ATheOneAIController* ATheOneGameModeBase::SpawnOneConfigAIAtTransform(const FTransform& SpawnTransform,
	const FTheOneAIPawnSpawnInfo& PawnSpawnInfo)
{
	auto DT = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable.LoadSynchronous();
	auto Config = DT->FindRow<FTheOneCharacterConfig>(PawnSpawnInfo.CharacterTemplateRowName, "SpawnOneConfigAIAtTransform");
	if (Config)
	{
		return SpawnOneAIAtTransform(Config->Custom.AIControllerClass, SpawnTransform, PawnSpawnInfo);
	}

	UE_LOG(LogTheOne, Error, TEXT("Failed to find CharacterConfig for %s"), *PawnSpawnInfo.CharacterTemplateRowName.ToString());
	return nullptr;
}

ATheOneAIController* ATheOneGameModeBase::SpawnOneAI(const TSubclassOf<ATheOneAIController> AIControllerClass,
                                                 const FTheOneAIPawnSpawnInfo& PawnSpawnInfo)
{
	return SpawnOneAIInternal(AIControllerClass, FTransform::Identity, PawnSpawnInfo, false);
}

ATheOneAIController* ATheOneGameModeBase::SpawnOneAIAtTransform(const TSubclassOf<ATheOneAIController> AIControllerClass,
	const FTransform& SpawnTransform, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo)
{
	return SpawnOneAIInternal(AIControllerClass, SpawnTransform, PawnSpawnInfo, true);
}

void ATheOneGameModeBase::RemoveOneAI(ATheOneAIController* InAIController)
{
	auto AIUniqueID = InAIController->GetUniqueID();
	RemoveOneAI(AIUniqueID);
}

void ATheOneGameModeBase::RemoveOneAICharacter(ACharacter* InAICharacter)
{
	auto AIController = Cast<ATheOneAIController>(InAICharacter->GetController());
	if (AIController)
	{
		RemoveOneAI(AIController);
	}
	else
	{
		UE_LOG(LogTheOne, Error, TEXT("AIController is nullptr for AICharacter %s"), *InAICharacter->GetName());
	}
}

void ATheOneGameModeBase::RemoveOneAI(uint32 AIUniqueID)
{
	UTheOneBlueprintFunctionLibrary::AssertIsTrue(SpawnedAIMap.Contains(AIUniqueID), FString::Printf(TEXT("AI with ID %d not found"), AIUniqueID));
	
	AAIController* BotToRemove = SpawnedAIMap[AIUniqueID];
	SpawnedAIMap.Remove(AIUniqueID);

	if (BotToRemove)
	{
		// just destroy the actor
		if (APawn* ControlledPawn = BotToRemove->GetPawn())
		{
			ControlledPawn->Destroy();
		}

		// Destroy the controller (will cause it to Logout, etc...)
		BotToRemove->Destroy();
	}
	else
	{
		UE_LOG(LogTheOne, Error, TEXT("BotToRemove AIController is nullptr, UniqueID: %d"), AIUniqueID);
	}
}

FString ATheOneGameModeBase::CreateAIName(int32 AIIndex)
{
	return FString("AI") + FString::FromInt(AIIndex);
}

void ATheOneGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (auto AutoChessCom = FindComponentByClass<UTheOneAutoChessBattleComponent>())
	{
		AutoChessBattleComponentCache = AutoChessCom;
	}
}

void ATheOneGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATheOneGameModeBase::StartPlay()
{
	Super::StartPlay();
	auto World = GetWorld();
	// 检查是否有HexGrid
	AHexGrid* HexGrid = nullptr;
	for (TActorIterator<AHexGrid> It(World); It; ++It)
	{
		HexGrid =  *It;
		GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid = HexGrid;
		break;
	}	


	if (HexGrid)
	{
		HexGrid->CreateGrid();
	
		for (TActorIterator<AHexAStarNavMesh> It(World); It; ++It)
		{
			It->SetHexGrid(HexGrid);
			// Todo: 监听事件设置为Static
			// Next Tick
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [It]()
			{
				It->SetToStatic();
			}, 0.f, false);
			break;
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("HexGrid Not Found"));
	}
	
	// 所有Actor已加载完毕
	OnLevelPrepared.Broadcast();
	BP_OnLevelPrepared();
}

ATheOneAIController* ATheOneGameModeBase::SpawnOneAIInternal(const TSubclassOf<ATheOneAIController> AIControllerClass,
	const FTransform& SpawnTransform, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo, bool UseTransform)
{
	// Todo: 传入AI的一些初始化数据， 如出生位置等 
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	ATheOneAIController* NewController = GetWorld()->SpawnActor<ATheOneAIController>(
		AIControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);

	if (NewController != nullptr)
	{
		auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
		ItemSystem->RegisterCharacterSlots(NewController->GetUniqueID());
		NewController->SetPawnSpawnInfo(PawnSpawnInfo);
		GenericPlayerInitialization(NewController);
		if (UseTransform)
		{
			RestartPlayerAtTransform(NewController, SpawnTransform);
		}
		else
		{
			RestartPlayer(NewController);
		}
		
		// 创建Pawn的流畅
		// 1、 RestartPlayer
		// 2、 FindPlayerStart
		// 3、 ChoosePlayerStart_Implementation 第一次执行GetDefaultPawnClassForController_Implementation
		// 4、 RestartPlayerAtPlayerStart
		// 5、 检查Pawn是否为null， 第二次执行GetDefaultPawnClassForController_Implementation
		// 6、 SpawnDefaultPawnAtTransform_Implementation， 创建Pawn， 第三次执行GetDefaultPawnClassForController_Implementation

		if (NewController->GetPawn() != nullptr)
		{
			// Todo: 一些需要手动调用的AI初始化函数
			auto TheOneCharacter = Cast<ATheOneCharacterBase>(NewController->GetPawn());
			NewController->WeakTheOneCharacterBase = TheOneCharacter;
			TheOneCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(TheOneCharacter, TheOneCharacter);
			TheOneCharacter->ConfigRowName = PawnSpawnInfo.CharacterTemplateRowName;
			if (TheOneCharacter)
			{
				auto MinionTemplate = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable->FindRow<FTheOneCharacterConfig>(TheOneCharacter->ConfigRowName, "SpawnOneAIInternal");
				if (MinionTemplate)
				{
					auto RootMesh = TheOneCharacter->GetMesh();
					RootMesh->SetAnimInstanceClass(MinionTemplate->Custom.AnimBP);
					RootMesh->SetRelativeLocation(MinionTemplate->Custom.MeshRootOffset);
					TheOneCharacter->HeadBarWidgetComponent->SetRelativeLocation(MinionTemplate->Custom.HealthBarOffset);
					if  (MinionTemplate->Custom.bIsRetarget)
					{
						// Mesh 设置为不可见，设置Tick模式为
						RootMesh->SetVisibility(false);
						RootMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

						// 创建附加SkeletonMeshComponent
						auto SkinMeshCom = NewObject<USkeletalMeshComponent>(TheOneCharacter);
						SkinMeshCom->RegisterComponent();
						SkinMeshCom->AttachToComponent(RootMesh, FAttachmentTransformRules::KeepRelativeTransform);
						SkinMeshCom->SetSkeletalMesh(MinionTemplate->Custom.SoftMainMesh.LoadSynchronous());
						SkinMeshCom->SetFlags(RF_Transactional);
						SkinMeshCom->SetAnimClass(MinionTemplate->Custom.RetargetAnimBP);

						// Todo: 打包时检查是否需要AddInstanceComponent和RegisterComponent
						TheOneCharacter->AddInstanceComponent(SkinMeshCom);
						TheOneCharacter->OutlookMesh = SkinMeshCom;
					}
					else
					{
						RootMesh->SetSkeletalMesh(MinionTemplate->Custom.SoftMainMesh.LoadSynchronous());
						TheOneCharacter->OutlookMesh = RootMesh;
					}

					TArray<FTheOneAdditionMesh> AdditionalMeshes = MinionTemplate->Custom.AdditionalMeshes;
					
					// 创建附加Mesh
					for (auto& Mesh : AdditionalMeshes)
					{
						UTheOneBlueprintFunctionLibrary::AttachAdditionMeshToCharacter(TheOneCharacter, Mesh);
					}

					// Give AttributeSet
					auto GSettings = GetDefault<UTheOneGeneralSettings>();
					auto SpecHandle = UTheOneBlueprintFunctionLibrary::SimpleMakeGESpecHandle(TheOneCharacter, GSettings->InitVitalAttributeSetEffect, 1);
					
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxHealth, MinionTemplate->Attr.MaxHealth);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxMana, MinionTemplate->Attr.MaxMana);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_HealthRegen, MinionTemplate->Attr.HealthRegen);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_ManaRegen, MinionTemplate->Attr.ManaRegen);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_Armor, MinionTemplate->Attr.Armor);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_MagicResistance, MinionTemplate->Attr.MagicResistance);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_MoveSpeed, MinionTemplate->Attr.MoveSpeed);
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_TurnSpeed, MinionTemplate->Attr.TurnSpeed);
					
					TheOneCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					TheOneCharacter->GetLifeAttributeSet()->SetHealth(TheOneCharacter->GetLifeAttributeSet()->GetMaxHealth());
					TheOneCharacter->GetLifeAttributeSet()->SetMana(TheOneCharacter->GetLifeAttributeSet()->GetMaxMana());
					
					auto ASC = TheOneCharacter->GetAbilitySystemComponent();
					auto CharacterAbilities = MinionTemplate->Ability.AbilityRows;
					for (const auto& RowName : CharacterAbilities)
					{
						FGameplayAbilitySpecHandle GAHandle;
						UTheOneGeneralGA* InstancedAbility = nullptr;
						if (UTheOneBlueprintFunctionLibrary::TheOneGiveAbility(ASC, RowName, GAHandle, InstancedAbility))
						{
							check(TheOneCharacter->CharacterAbilityMap.Contains(GAHandle) == false);
							TheOneCharacter->CharacterAbilityMap.Add(GAHandle, InstancedAbility);
						}
					}

					// 给与数据驱动通用被动技能
					auto Spec = FGameplayAbilitySpec(GSettings->DefaultDTDrivePassiveGA, 1);
					FGameplayAbilitySpecHandle GAHandle = ASC->GiveAbility(Spec);
					auto InstancedAbility = Cast<UTheOneDataDrivePassiveGA>(ASC->FindAbilitySpecFromHandle(GAHandle)->GetPrimaryInstance());
					InstancedAbility->SetupDataDrivePassiveAbility();
					TheOneCharacter->DataDrivePassiveSpecHandle = GAHandle;
					TheOneCharacter->DataDrivePassiveGA = InstancedAbility;
					bool ActiveDataDrivePassive =  ASC->TryActivateAbility(GAHandle);
					check(ActiveDataDrivePassive);

					// 装备默认武器
					TheOneCharacter->DefaultWeaponRow = MinionTemplate->DefaultWeaponConfigRow.RowName;
					UTheOneBlueprintFunctionLibrary::EquipWeapon(TheOneCharacter, INDEX_NONE);
				}
				else
				{
					UE_LOG(LogTheOne, Error, TEXT("MinionTemplate is nullptr For MinionRow: %s"), *PawnSpawnInfo.CharacterTemplateRowName.ToString());
				}
				TheOneCharacter->SetCamp(PawnSpawnInfo.Camp);
				TheOneCharacter->SpawnInit();
			}
		}
		else
		{
			UE_LOG(LogTheOne, Error, TEXT("AIController %s has no pawn"), *NewController->GetName());
		}
		
		SpawnedAIMap.Add(NewController->GetUniqueID(), NewController);
	}

	return NewController;
}

