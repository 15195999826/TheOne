// Fill out your copyright notice in the Description page of Project Settings.


#include "TheOneBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "TheOneGlobal.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAbilityAction.h"
#include "AbilitySystem/Abilities/TheOneDataDrivePassiveGA.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/TheOneCharacterBase.h"
#include "Development/TheOneGeneralSettings.h"
#include "Framework/Application/NavigationConfig.h"
#include "Game/TheOneGameInstance.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"
#include "Interface/TheOneCharacterABPInterface.h"
#include "Item/TheOneItemSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneContextSystem.h"
#include "Types/TheOneItem.h"
#include "UI/TheOneUIRoot.h"

void UTheOneBlueprintFunctionLibrary::AssertIsTrue(bool b, const FString& Message)
{
	if (!b)
	{
		auto ErrorMessage = FString::Printf(TEXT("AssertIsTrue fail: %s"), *Message);
		UE_LOG(LogTheOne, Error, TEXT("%s"), *ErrorMessage);
		// 终止游戏运行
		FGenericPlatformMisc::RequestExit(false, *ErrorMessage);
	}
}

bool UTheOneBlueprintFunctionLibrary::IsCharacterConfigValid(FName InRowName)
{
	auto DT = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable.LoadSynchronous();
	auto FindRow = DT->FindRow<FTheOneCharacterConfig>(InRowName, "UTheOneBlueprintFunctionLibrary::IsCharacterConfigValid");
	if (FindRow == nullptr)
	{
		UE_LOG(LogTheOne, Error, TEXT("CharacterConfig %s is not valid."), *InRowName.ToString());
		return false;
	}

	if (FindRow->Custom.MinionClass == nullptr ||
		FindRow->Custom.AIControllerClass == nullptr
	)
	{
		UE_LOG(LogTheOne, Error, TEXT("CharacterConfig %s CharacterClass is not valid."), *InRowName.ToString());
		return false;
	}

	return true;
}

FGameplayEffectSpecHandle UTheOneBlueprintFunctionLibrary::SimpleMakeGESpecHandle(AActor* Source,
                                                                                TSubclassOf<UGameplayEffect> GEClass, float Level)
{
	const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source);
	check(ASC)
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Source);
	
	return ASC->MakeOutgoingSpec(GEClass, Level, EffectContextHandle);
}

FActiveGameplayEffectHandle UTheOneBlueprintFunctionLibrary::SimpleApplyGEToSelf(AActor* Source,
                                                                               TSubclassOf<UGameplayEffect> GE, float Level)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source);
	if (ASC == nullptr)
	{
		return EmptyActiveGameplayEffectHandle;
	}

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Source);
	EffectContextHandle.AddInstigator(Source, Source);

	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GE, Level, EffectContextHandle);
	return ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

ATheOneCharacterBase* UTheOneBlueprintFunctionLibrary::FindCharacter(const UObject* WorldContextObject, uint32 InFlag)
{
	auto TheOneGM = Cast<ATheOneGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (TheOneGM)
	{
		if (TheOneGM->SpawnedAIMap.Contains(InFlag))
		{
			return TheOneGM->SpawnedAIMap[InFlag]->WeakTheOneCharacterBase.Get();
		}
	}
	return nullptr;
}

AHexGrid* UTheOneBlueprintFunctionLibrary::GetHexGrid(const UObject* WorldContextObject)
{
	auto ContextSystem = WorldContextObject->GetWorld()->GetSubsystem<UTheOneContextSystem>();
	if (ContextSystem->HexGrid == nullptr)
	{
		UE_LOG(LogTheOne, Error, TEXT("HexMapContextSystem HexGrid is not valid."));
		return nullptr;
	}

	return ContextSystem->HexGrid;
}

bool UTheOneBlueprintFunctionLibrary::TheOneGiveAbility(UAbilitySystemComponent* InASC, const FName& InRowName, FGameplayAbilitySpecHandle& OutGAHandle, UTheOneGeneralGA*& OutGAInstance)
{
	auto AbilityDT = GetDefault<UTheOneDataTableSettings>()->AbilityTable.LoadSynchronous();
	FTheOneAbilityConfig* AbilityRow = AbilityDT->FindRow<FTheOneAbilityConfig>(
							InRowName, "SpawnOneAIInternal");
	if (AbilityRow)
	{
		FDataTableRowHandle AbilityRowHandle;
		AbilityRowHandle.RowName = InRowName;
		AbilityRowHandle.DataTable = AbilityDT;
		FGameplayAbilitySpecHandle GAHandle = InASC->GiveAbility(FGameplayAbilitySpec(AbilityRow->AbilityClass, 1));

		UTheOneGeneralGA* InstancedAbility = Cast<UTheOneGeneralGA>(
			InASC->FindAbilitySpecFromHandle(GAHandle)->GetPrimaryInstance());
		InstancedAbility->SetUpGeneralAbility(AbilityRowHandle, AbilityRow, 1);

		if (AbilityRow->AbilityType == ETheOneAbilityType::Passive)
		{
			if (!InASC->TryActivateAbility(GAHandle))
			{
				UE_LOG(LogTheOne, Error, TEXT("Failed to activate Passive Ability %s For Owner %s"), *InRowName.ToString(), *InASC->GetOwner()->GetName());
				return false;
			}
		}

		OutGAHandle = GAHandle;
		OutGAInstance = InstancedAbility;
		return true;
	}

	UE_LOG(LogTheOne, Error, TEXT("AbilityRow is nullptr For RowName: %s"), *InRowName.ToString());

	return false;
}

void UTheOneBlueprintFunctionLibrary::EquipWeapon(ATheOneCharacterBase* InCharacter, int32 InWeaponID)
{
	if (InCharacter->AttackGA.IsValid() && InWeaponID == InCharacter->WeaponID)
	{
		UE_LOG(LogTheOne, Error, TEXT("Character %s has already equipped Weapon %d."), *InCharacter->GetName(), InWeaponID);
		return;
	}

	UnEquipWeapon(InCharacter);
	FTheOneWeaponConfig* FindRow;
	auto DT = GetDefault<UTheOneDataTableSettings>()->WeaponTable.LoadSynchronous();
	if (InWeaponID != INDEX_NONE)
	{
		auto Weapon = InCharacter->GetWorld()->GetSubsystem<UTheOneItemSystem>()->FindItem(InWeaponID);
		check(Weapon)
	
		// 检查角色是否装备了武器, 如果装备了武器，先走UnEquipWeapon
		FindRow = DT->FindRow<FTheOneWeaponConfig>(Weapon->ItemRowName,"UTheOneBlueprintFunctionLibrary::EquipWeapon");
	}
	else
	{
		FindRow = DT->FindRow<FTheOneWeaponConfig>(InCharacter->DefaultWeaponRow, "UTheOneBlueprintFunctionLibrary::EquipDefaultWeapon");
	}
	check(FindRow)

	for (const auto& AdditionMesh : FindRow->WeaponMesh)
	{
		AttachAdditionMeshToCharacter(InCharacter, AdditionMesh, TheOneGlobal::WeaponMeshTag);
	}
	
	auto ASC = InCharacter->GetAbilitySystemComponent();
	InCharacter->WeaponID = InWeaponID;

	// 覆写角色攻击力、攻击速度、攻击范围
	auto GSettings = GetDefault<UTheOneGeneralSettings>();
	auto SpecHandle = SimpleMakeGESpecHandle(InCharacter, GSettings->InitWeaponVitalAttributeSetEffect, 1);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_Attack, FindRow->AttackAbility.Attack);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_AttackRange, FindRow->AttackAbility.AttackRange);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Attribute_Vital_AttackSpeed, FindRow->AttackAbility.AttackSpeed);
				
	InCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	// 给与普攻
	auto GeneralSettings = GetDefault<UTheOneGeneralSettings>();
	InCharacter->AttackAbilitySpecHandle = ASC->GiveAbility(FGameplayAbilitySpec(GeneralSettings->DefaultAttackGA, 1));
	auto InstancedAbility = Cast<UTheOneAttackGA>(ASC->FindAbilitySpecFromHandle(InCharacter->AttackAbilitySpecHandle)->GetPrimaryInstance());
	InstancedAbility->SetupAttackAbility(FindRow->AttackAbility);
	InCharacter->AttackGA = InstancedAbility;
	
	// 给予其它武器技能
	UTheOneGeneralGA* WeaponAbilityA = nullptr;
	if (TheOneGiveAbility(ASC, FindRow->AbilityA.RowName, InCharacter->WeaponAbilityASpecHandle, WeaponAbilityA))
	{
		InCharacter->WeakWeaponAbilityA = WeaponAbilityA;
	}
	
	UTheOneGeneralGA* WeaponAbilityB = nullptr;
	if (TheOneGiveAbility(ASC, FindRow->AbilityB.RowName, InCharacter->WeaponAbilityBSpecHandle, WeaponAbilityB))
	{
		InCharacter->WeakWeaponAbilityB = WeaponAbilityB;
	}

	UTheOneGeneralGA* WeaponAbilityC = nullptr;
	if (TheOneGiveAbility(ASC, FindRow->AbilityC.RowName, InCharacter->WeaponAbilityCSpecHandle, WeaponAbilityC))
	{
		InCharacter->WeakWeaponAbilityC = WeaponAbilityC;
	}

	// 更新动画
	auto AnimInstance = InCharacter->GetMesh()->GetAnimInstance();
	ITheOneCharacterABPInterface::Execute_OnChangeWeapon(AnimInstance, FindRow->AnimBPConfigRow);
}

void UTheOneBlueprintFunctionLibrary::UnEquipWeapon(ATheOneCharacterBase* InCharacter)
{
	auto ASC = InCharacter->GetAbilitySystemComponent();
	if (InCharacter->AttackAbilitySpecHandle.IsValid())
	{
		// 移除技能
		ASC->ClearAbility(InCharacter->AttackAbilitySpecHandle);
	}
	if (InCharacter->WeaponAbilityASpecHandle.IsValid())
	{
		// 移除技能
		ASC->ClearAbility(InCharacter->WeaponAbilityASpecHandle);
	}
	if (InCharacter->WeaponAbilityBSpecHandle.IsValid())
	{
		// 移除技能
		ASC->ClearAbility(InCharacter->WeaponAbilityBSpecHandle);
	}
	if (InCharacter->WeaponAbilityCSpecHandle.IsValid())
	{
		// 移除技能
		ASC->ClearAbility(InCharacter->WeaponAbilityCSpecHandle);
	}

	TArray<USceneComponent*> Children;
	InCharacter->OutlookMesh->GetChildrenComponents(true,Children);

	for (auto Child : Children)
	{
		if (Child->ComponentHasTag(TheOneGlobal::WeaponMeshTag))
		{
			Child->DestroyComponent();
		}
	}
}

void UTheOneBlueprintFunctionLibrary::AttachAdditionMeshToCharacter(ATheOneCharacterBase* InCharacter,
	const FTheOneAdditionMesh& InAdditionMesh, FName InTag)
{
	USkeletalMeshComponent* AttachTo = InAdditionMesh.AttachToOutlook? InCharacter->OutlookMesh.Get() : InCharacter->GetMesh();
	
	auto AdditionMeshCom = NewObject<USkeletalMeshComponent>(InCharacter);
	AdditionMeshCom->RegisterComponent();
	AdditionMeshCom->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform, InAdditionMesh.SocketName);
	AdditionMeshCom->SetRelativeLocation(FVector::Zero());
	AdditionMeshCom->SetSkeletalMesh(InAdditionMesh.Mesh.LoadSynchronous());
	AdditionMeshCom->SetFlags(RF_Transactional);

	if (InTag != NAME_None)
	{
		AdditionMeshCom->ComponentTags.Add(InTag);
	}

	// Todo: If Leader Pose
	InCharacter->AddInstanceComponent(AdditionMeshCom);
}

FGameplayTag UTheOneBlueprintFunctionLibrary::CastToDamageTypeTag(ETheOneDamageType InDamageType)
{
	switch (InDamageType)
	{
		case ETheOneDamageType::Physical:
			return TheOneGameplayTags::SetByCaller_Damage_Physical;
		case ETheOneDamageType::Magical:
			return TheOneGameplayTags::SetByCaller_Damage_Magical;
		case ETheOneDamageType::Real:
			return TheOneGameplayTags::SetByCaller_Damage_Real;
	}

	UE_LOG(LogTheOne, Error, TEXT("CastToDamageTypeTag Fail: %d, 设置成了物理攻击"), InDamageType);
	return TheOneGameplayTags::SetByCaller_Damage_Physical;
}

FName UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType InFlyType)
{
	switch (InFlyType)
	{
		case ETheOneProjectileFlyType::Trace:
			return TEXT("追踪");
		case ETheOneProjectileFlyType::FixedLocation:
			return TEXT("固定位置");
		case ETheOneProjectileFlyType::Lob:
			return TEXT("抛物线");
		case ETheOneProjectileFlyType::Line:
			return TEXT("直线");
		case ETheOneProjectileFlyType::Instant:
			return TEXT("瞬发");
	}

	return TEXT("无效");
}

ETheOneProjectileFlyType UTheOneBlueprintFunctionLibrary::NameToProjectileFlyType(FName InName)
{
	if (InName == TEXT("追踪"))
	{
		return ETheOneProjectileFlyType::Trace;
	}
	if (InName == TEXT("固定位置"))
	{
		return ETheOneProjectileFlyType::FixedLocation;
	}
	if (InName == TEXT("抛物线"))
	{
		return ETheOneProjectileFlyType::Lob;
	}
	if (InName == TEXT("直线"))
	{
		return ETheOneProjectileFlyType::Line;
	}
	if (InName == TEXT("瞬发"))
	{
		return ETheOneProjectileFlyType::Instant;
	}

	return ETheOneProjectileFlyType::Invalid;
}

bool UTheOneBlueprintFunctionLibrary::TryFindSocketLocation(AActor* InActor, const FName& InSocketName,
	FVector& OutLocation)
{
	if (InActor && InActor->Implements<UInHexActorInterface>())
	{
		auto RootMesh = IInHexActorInterface::Execute_GetRootMesh(InActor);
		if (RootMesh)
		{
			if (RootMesh->DoesSocketExist(InSocketName))
			{
				OutLocation = RootMesh->GetSocketLocation(InSocketName);
				return true;
			}

			// 获取全部Child
			TArray<USceneComponent*> Children;
			RootMesh->GetChildrenComponents(true, Children);
			for (auto Child : Children)
			{
				if (Child->DoesSocketExist(InSocketName))
				{
					OutLocation = Child->GetSocketLocation(InSocketName);
					return true;
				}
			}
		}
	}

	return false;
}

FTheOneAbilityActionPoint UTheOneBlueprintFunctionLibrary::CastConfigToAbilityActionPoint(const FTheOneActionPointConfig& InActionPoint)
{
	FTheOneAbilityActionPoint Ret;
	Ret.ActionPoint = InActionPoint.ActionPoint;
	for (const auto& ActionContainer : InActionPoint.Actions)
	{
		Ret.Actions.Add(CastConfigToAbilityActionContainer(ActionContainer));
	}

	return Ret;
}

FTheOneAbilityActionContainer UTheOneBlueprintFunctionLibrary::CastConfigToAbilityActionContainer(
	const FTheOneAbilityActionContainerConfig& InActionContainer)
{
	FTheOneAbilityActionContainer Ret;
	Ret.TargetSelector = InActionContainer.TargetSelector;
	for (const auto& Action : InActionContainer.Actions)
	{
		Ret.Actions.Add(CastConfigToAbilityAction(Action));
	}

	return Ret;
}

FTheOneAbilityAction UTheOneBlueprintFunctionLibrary::CastConfigToAbilityAction(const FTheOneAbilityActionConfig& InAction)
{
	FTheOneAbilityAction Ret;
	Ret.ActionData = CastConfigToAbilityActionData(InAction.ActionData);
	for (const auto& ActionContainer : InAction.ForBindActionParam)
	{
		Ret.ForBindActionParam.Add(ActionContainer.Key, CastConfigToAbilityActionData(ActionContainer.Value));
	}

	return Ret;
}

FTheOneAbilityActionData UTheOneBlueprintFunctionLibrary::CastConfigToAbilityActionData(
	const FTheOneAbilityActionDataConfig& InActionData)
{
	FTheOneAbilityActionData Ret;
	Ret.ActionType = InActionData.ActionType;
	for (const auto& Pair : InActionData.CustomKVs)
	{
		auto& Value = Pair.Value;
		if (Pair.Key == FTheOneAbilityActionDataConfig::ExpressionKey)
		{
			// Split By |
			Value.ToString().ParseIntoArray(Ret.Expressions, TEXT("|"), true);
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::DamageTagKey)
		{
			Ret.DamageTag = FGameplayTag::RequestGameplayTag(Value);
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::ProjectileRowNameKey)
		{
			Ret.ProjectileRowName = Value;
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::SpawnSocketNameKey)
		{
			Ret.SpawnSocketName = Value;
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::SpawnOffsetKey)
		{
			// FName To FVector
			FVector OutVector;
			bool Success = OutVector.InitFromString(Value.ToString());
			if (!Success)
			{
				UE_LOG(LogTheOne, Error, TEXT("CastToRuntimeAbilityActionData SpawnOffsetKey InitFromString Fail: %s"), *Value.ToString());
			}
			Ret.SpawnOffset = OutVector;
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::FlySpeedKey)
		{
			// FName To Float
			Ret.FlySpeed =  FCString::Atof(*Value.ToString());
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::FlyTypeKey)
		{
			Ret.FlyType = NameToProjectileFlyType(Value);
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::HitActionsKey)
		{
			Value.ToString().ParseIntoArray(Ret.HitActionKeys, TEXT("|"), true);
		}
		else if (Pair.Key == FTheOneAbilityActionDataConfig::BuffRowNameKey)
		{
			Ret.BuffRowName = Value;
		}
	}

	return Ret;
}

void UTheOneBlueprintFunctionLibrary::EnableUINav()
{
	FNavigationConfig& NavConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavConfig.bTabNavigation = true;
}

void UTheOneBlueprintFunctionLibrary::DisableUINav()
{
	FNavigationConfig& NavConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavConfig.bTabNavigation = false;
}

TArray<AActor*> UTheOneBlueprintFunctionLibrary::GetHitBoxHitResult(const UObject* WorldContextObject,
                                                                  const FVector& SourceLocation,
                                                                  const FVector& SourceForward,
                                                                  const FRotator& SourceRotation,
                                                                  ETheOneHiBoxType HitBoxType, bool HitMulti,
                                                                  const FVector& BoxSize, float SectorAngle,
                                                                  float Radius, float AboveGround,
                                                                  float StartDistance, float EndDistance,
                                                                  bool IgnoreSelf,
                                                                  bool DrawDebug)
{
	
	FVector StartLocation = SourceLocation + FVector(0,0,1) * AboveGround + SourceForward * StartDistance;

	// 设置射线追踪的结束位置
	FVector EndLocation = SourceLocation + FVector(0,0,1) * AboveGround + SourceForward * EndDistance;

	// 设置需要忽略的Actor
	TArray<AActor*> ActorsToIgnore;
	
	// Trace Object Types
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	// 加入Pawn
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> InRangeHitActors;
	if (HitMulti)
	{
		// 扇形方位检测方式 : 先射线检测到圆形半径内的所有Actor， 再根据这些Actor的位置与圆心位置进行角度检测
		switch (HitBoxType)
		{
			case ETheOneHiBoxType::Box:
				BoxHits(WorldContextObject, SourceRotation, StartLocation, EndLocation, BoxSize, ActorsToIgnore,
				        ObjectTypes, InRangeHitActors, IgnoreSelf, DrawDebug);
				break;
			case ETheOneHiBoxType::Circle:
				CircleHits(WorldContextObject, StartLocation, Radius, ActorsToIgnore, ObjectTypes, InRangeHitActors,
				           IgnoreSelf, DrawDebug);
				break;
			case ETheOneHiBoxType::Sector:
				SectorHits(WorldContextObject, SourceForward, StartLocation, SectorAngle, Radius, ActorsToIgnore,
				           ObjectTypes, InRangeHitActors, IgnoreSelf, DrawDebug);
				break;
		}
	}
	else
	{
		if (HitBoxType == ETheOneHiBoxType::Circle || HitBoxType == ETheOneHiBoxType::Sector)
		{
			// Todo:
			UE_LOG(LogTemp, Error, TEXT("[GetHitBoxHitResult] SingleHit HitBoxType::Circle or Sector is not implemented yet."));
		}
		else
		{
			FHitResult OutHit;
			EDrawDebugTrace::Type DrawDebugType = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
			if (UKismetSystemLibrary::BoxTraceSingleForObjects(WorldContextObject, StartLocation, EndLocation, BoxSize,
				SourceRotation, ObjectTypes, true,
				ActorsToIgnore, DrawDebugType, OutHit, IgnoreSelf,
				FLinearColor::White, FLinearColor::Red, 0.5f))
			{
			
				AActor* HitActor = OutHit.GetActor();
				if (HitActor)
				{
					InRangeHitActors.Add(HitActor);
				}
			}
		}
	}

	return InRangeHitActors;
}

void UTheOneBlueprintFunctionLibrary::BoxHits(const UObject* WorldContextObject, const FRotator& Orientation,
                                            const FVector& StartLocation, const FVector& EndLocation, const FVector& BoxSize, TArray<AActor*> ActorsToIgnore,
                                            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*>& InRangeHitActors, bool IgnoreSelf, bool DrawDebug)
{
	TArray<FHitResult> OutHits;

	EDrawDebugTrace::Type DrawDebugType = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, StartLocation, EndLocation, BoxSize,
												 Orientation , ObjectTypes, true,
												 ActorsToIgnore,DrawDebugType, OutHits, IgnoreSelf,
												 FLinearColor::White, FLinearColor::Red, 0.5f);
	
	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			if (!InRangeHitActors.Contains(HitActor))
			{
				InRangeHitActors.Add(HitActor);
			}
		}
	}
}

void UTheOneBlueprintFunctionLibrary::SectorHits(const UObject* WorldContextObject, const FVector& Forward,
                                               const FVector& StartLocation, float SectorAngle, float Radius, TArray<AActor*> ActorsToIgnore,
                                               TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*>& InRangeHitActors, bool IgnoreSelf, bool DrawDebug)
{
	TArray<FHitResult> OutHits;
	EDrawDebugTrace::Type DrawDebugType = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	UKismetSystemLibrary::SphereTraceMultiForObjects(WorldContextObject, StartLocation, StartLocation, Radius,
													 ObjectTypes, true,
													 ActorsToIgnore, DrawDebugType, OutHits,
													 IgnoreSelf,
													 FLinearColor::White, FLinearColor::Red, 0.5f);
	// 绘制圆心
	if (DrawDebug)
	{
		auto World = WorldContextObject->GetWorld();
		DrawDebugSphere(World, StartLocation, 5, 10, FColor::Green, false, 0.5f);
		// 从圆心开始绘制扇形两条边界线， 扇形角度为SectorAngle
		FVector StartLine = StartLocation + Forward.RotateAngleAxis(SectorAngle / 2, FVector(0, 0, 1)) * Radius;
		FVector EndLine = StartLocation + Forward.RotateAngleAxis(-SectorAngle / 2, FVector(0, 0, 1)) * Radius;
		DrawDebugLine(World, StartLocation, StartLine, FColor::Green, false, 0.5f);
		DrawDebugLine(World, StartLocation, EndLine, FColor::Green, false, 0.5f);
	}
	
	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (InRangeHitActors.Contains(HitActor))
		{
			continue;
		}
		
		if (HitActor)
		{
			// 计算Actor与圆心的角度
			FVector HitLocation = HitActor->GetActorLocation();
			FVector Direction = HitLocation - StartLocation;
			Direction.Normalize();
			float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, Direction)));
			// 如果角度小于等于SectorAngle / 2， 则认为在扇形范围内
			if (Angle <= SectorAngle / 2)
			{
				InRangeHitActors.Add(HitActor);
			}
		}
	}
}

void UTheOneBlueprintFunctionLibrary::CircleHits(const UObject* WorldContextObject, const FVector& StartLocation,
	float Radius, TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
	TArray<AActor*>& InRangeHitActors, bool IgnoreSelf, bool DrawDebug)
{
	TArray<FHitResult> OutHits;
	EDrawDebugTrace::Type DrawDebugType = DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	UKismetSystemLibrary::SphereTraceMultiForObjects(WorldContextObject, StartLocation, StartLocation, Radius,
													 ObjectTypes, true,
													 ActorsToIgnore, DrawDebugType, OutHits,
													 IgnoreSelf,
													 FLinearColor::White, FLinearColor::Red, 0.5f);
	// 绘制圆心
	if (DrawDebug)
	{
		DrawDebugSphere(WorldContextObject->GetWorld(), StartLocation, 5, 10, FColor::Green, false, 0.5f);
	}

	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			if (!InRangeHitActors.Contains(HitActor))
			{
				InRangeHitActors.Add(HitActor);
			}
		}
	}
}

void UTheOneBlueprintFunctionLibrary::PushUI(const UObject* WorldContextObject, ETheOneUIOverlayType OverlayType,
	UUserWidget* Widget, TEnumAsByte<EHorizontalAlignment> HorizontalAlignment,
	TEnumAsByte<EVerticalAlignment> VerticalAlignment)
{
	auto TheOneGI = Cast<UTheOneGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	TheOneGI->UIRoot->PushUI(OverlayType, Widget, HorizontalAlignment, VerticalAlignment);
}

