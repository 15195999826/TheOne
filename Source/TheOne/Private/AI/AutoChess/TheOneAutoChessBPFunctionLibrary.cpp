// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/TheOneAutoChessBPFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "TheOneValidEnum.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/GameModeComponents/TheOneAutoChessBattleComponent.h"
#include "HexGrid/HexGrid.h"

UAutoChessAIBBContext* UTheOneAutoChessBPFunctionLibrary::GetAIBBContext(AAIController* InController)
{
	if (auto AutoChessAICtrl = Cast<ATheOneAutoChessAIController>(InController))
	{
		return AutoChessAICtrl->AIBBContext;
	}

	return nullptr;
}

AActor* UTheOneAutoChessBPFunctionLibrary::HasActorStandingOnGrid(const UObject* WorldContextObject,
	const FVector& InLocation, ETheOneValid& IsValid)
{
	AHexGrid* HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(WorldContextObject);
	if (HexGrid == nullptr)
	{
		IsValid = ETheOneValid::Invalid;
		return nullptr;
	}

	const auto& HexTile = HexGrid->GetHexTile(InLocation);
	if (HexTile.HasActor())
	{
		IsValid = ETheOneValid::Valid;
		return HexTile.StandingActor.Get();
	}

	IsValid = ETheOneValid::Invalid;
	return nullptr;
}

bool UTheOneAutoChessBPFunctionLibrary::GetRandomEmptyLocationOnHexGrid(const UObject* WorldContextObject,
	FVector& OutLocation, const FVector& InLocation, int32 Radius)
{
	auto HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(WorldContextObject);
	if (HexGrid == nullptr)
	{
		return false;
	}

	TArray<FHexTile> InRangeHexes = HexGrid->GetRange(HexGrid->WorldToHex(InLocation), Radius);
	TArray<int> EmptyHexIndexes;
	for (int i = 0; i < InRangeHexes.Num(); i++)
	{
		if (!InRangeHexes[i].HasActor())
		{
			EmptyHexIndexes.Add(i);
		}
	}

	if (EmptyHexIndexes.Num() == 0)
	{
		return false;
	}

	int32 RandomIndex = FMath::RandRange(0, EmptyHexIndexes.Num() - 1);
	OutLocation = HexGrid->HexToWorld(InRangeHexes[EmptyHexIndexes[RandomIndex]].CubeCoord);
	return true;
}

void UTheOneAutoChessBPFunctionLibrary::SelectAbilityAndTarget(AAIController* SelfAI, UAutoChessAIBBContext* InContext, ETheOneValid& IsValid)
{
	// auto HexAICtrl = Cast<ATheOneAutoChessAIController>(SelfAI);
	// if (HexAICtrl == nullptr)
	// {
	// 	IsValid = ETheOneValid::Invalid;
	// 	return;
	// }
	//
	// auto TheOneCharacter = Cast<ATheOneCharacterBase>(HexAICtrl->GetPawn());
	//
	// if (TheOneCharacter == nullptr)
	// {
	// 	return;
	// }
	//
	// check(TheOneCharacter->GetSkillState() != ETheOneCharacterSkillState::Casting)
	//
	// auto World = HexAICtrl->GetWorld();
	// auto TheOneGM = Cast<ATheOneGameModeBase>(World->GetAuthGameMode());
	// if (TheOneGM == nullptr)
	// {
	// 	return;
	// }
	//
	// UTheOneAutoChessBattleComponent* AutoChessCom = TheOneGM->GetAutoChessBattleComponent();
	// if (AutoChessCom == nullptr)
	// {
	// 	return;
	// }
	// // 检查是否已经死亡
	// bool RetargetByTargetIsDead = InContext->TargetAI != nullptr && InContext->TargetAI->IsDead();
	// // 检查当前是否已经设置了目标
	// if (RetargetByTargetIsDead || InContext->TargetAI == nullptr || InContext->ForceRetarget)
	// {
	// 	// 选择最近的敌人作为目标
	// 	auto SelfCoord = HexAICtrl->GetCurrentHexCoord();
	// 	int MinDistance = INT_MAX;
	// 	ATheOneAutoChessAIController* NearestEnemy = nullptr;
	//
	// 	auto Enemies = AutoChessCom->GetEnemies(TheOneCharacter);
	// 	for (TWeakObjectPtr<ATheOneAutoChessAIController> Enemy : Enemies)
	// 	{
	// 		if (!Enemy.IsValid()) continue;
	// 		if (Enemy->IsDead()) continue;
	// 		auto CurrentCoord = Enemy->GetCurrentHexCoord();
	// 		auto Distance = AHexGrid::GetDistance(SelfCoord, CurrentCoord);
	// 		if (Distance < MinDistance)
	// 		{
	// 			MinDistance = Distance;
	// 			NearestEnemy = Enemy.Get();
	// 		}
	// 	}
	//
	// 	InContext->SetTargetAI(NearestEnemy);
	// }
	//
	// if (InContext->TargetAI == nullptr)
	// {
	// 	IsValid = ETheOneValid::Invalid;
	// 	return;
	// }
	//
	// // 检查MainAbility是否是主动技能
	// UTheOneGameplayAbility* MainAbility = TheOneCharacter->GetMainAbility();
	// auto Attr = TheOneCharacter->GetAttributeSet();
	// if (MainAbility->GetAbilityType() == ETheOneAbilityType::Active && Attr->GetMana() >= Attr->GetMaxMana())
	// {
	// 	// 检查蓝量是否满了，满了就释放MainAbility，否则释放NormalAbility
	// 	InContext->SelectedAbilitySpecHandle = TheOneCharacter->MainAbilitySpecHandle;
	// }
	// else
	// {
	// 	InContext->SelectedAbilitySpecHandle = TheOneCharacter->NormalAbilitySpecHandle;
	// }
	//
	IsValid = ETheOneValid::Valid;
}

float UTheOneAutoChessBPFunctionLibrary::RotateToActor(AActor* SourceActor, AActor* TargetActor, float DeltaTime)
{
	if (SourceActor == nullptr || TargetActor == nullptr)
	{
		return 0.f;
	}
	
	FVector TargetPosition = TargetActor->GetActorLocation();
	return RotateToPoint(SourceActor, TargetPosition, DeltaTime);
}

float UTheOneAutoChessBPFunctionLibrary::RotateToPoint(AActor* SourceActor, const FVector& TargetPoint, float DeltaTime)
{
	auto SourcePosition = SourceActor->GetActorLocation();
	// 计算目标方向的向量
	FVector DirectionVector = TargetPoint - SourcePosition;

	// 计算向量的角度
	float AngleRad = FMath::Atan2(DirectionVector.Y, DirectionVector.X);

	// 将弧度转换为角度
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	bool Find = false;
	float TurnSpeed =  UAbilitySystemBlueprintLibrary::GetFloatAttribute(SourceActor, UTheOneAttributeSet::GetTurnSpeedAttribute(), Find);
	// 创建一个新的旋转器，只改变Yaw轴
	FRotator NewRotation = FRotator(0, AngleDeg, 0);
	if (Find)
	{
		auto MaxTurnAngle = TurnSpeed * DeltaTime;
		// 计算自己转到NewRotation需要旋转多少
		auto DeltaYaw = FRotator::NormalizeAxis(NewRotation.Yaw - SourceActor->GetActorRotation().Yaw);
		// 如果DeltaYaw大于MaxTurnAngle，就只转MaxTurnAngle
		if (FMath::Abs(DeltaYaw) > MaxTurnAngle)
		{
			NewRotation.Yaw = SourceActor->GetActorRotation().Yaw + FMath::Sign(DeltaYaw) * MaxTurnAngle;
		}

		// 设置角色的旋转
		SourceActor->SetActorRotation(NewRotation);
	}
	else
	{
		// 设置角色的旋转
		SourceActor->SetActorRotation(NewRotation);
	}

	return AngleDeg;
}

bool UTheOneAutoChessBPFunctionLibrary::JumpToPoint(AActor* SourceActor, const FVector& FromPoint,
	const FVector& TargetPoint, float TargetHeight,float MaxHeight, float DeltaTime, float Speed)
{
	auto ActorLocation = SourceActor->GetActorLocation();
	ActorLocation.Z = 0;
	auto TotalDistance = FVector::Dist(FromPoint, TargetPoint);
	auto RemainingDistance = FVector::Dist(ActorLocation, TargetPoint);

	auto WantDistance = RemainingDistance - Speed * DeltaTime;

	if (WantDistance <= 0)
	{
		return true;
	}
	
	auto WantPercent =1.f - WantDistance / TotalDistance;
	// 根据百分比计算当前高度, WantPercent为0时，高度为0，为0.5时，高度为MaxHeight，为1时，高度为0， sin函数满足这个条件
	auto WantHeight = FMath::Sin(WantPercent * PI) * MaxHeight;
	// 计算新的位置
	auto NewLocation = FMath::Lerp(FromPoint, TargetPoint, WantPercent);
	SourceActor->SetActorLocation(NewLocation + FVector(0, 0, WantHeight + TargetHeight));
	
	return false;
}

