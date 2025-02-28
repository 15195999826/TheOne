// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Battle/TheOneBattleEvaluate.h"

#include "AIController.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "Character/TheOneCharacterBase.h"
#include "Game/TheOneBattle.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"
#include "HexGrid/HexGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneContextSystem.h"

// Sets default values for this component's properties
ATheOneBattleEvaluate::ATheOneBattleEvaluate()
{
}


// Called when the game starts
void ATheOneBattleEvaluate::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

double ATheOneBattleEvaluate::CallEvaFunctionByName(const FName& InFunctionName, const FTheOneEvaParams& InParams)
{
	float Result = 0.0f;
	auto Fuc = this->FindFunction(InFunctionName);
	if (Fuc)
	{
		struct FMyBlueprintFunctionParams
		{
			FTheOneEvaParams Params;
			double ReturnValue;
		};

		FMyBlueprintFunctionParams Params;
		Params.Params = InParams;
		this->ProcessEvent(Fuc, &Params);
		UE_LOG(LogTemp, Warning, TEXT("ATheOneBattleEvaluate::CallEvaFunctionByName %s, ReturnValue %f"), *InFunctionName.ToString(), Params.ReturnValue);

		Result = Params.ReturnValue;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATheOneBattleEvaluate Function %s not found"), *InFunctionName.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("ATheOneBattleEvaluate::CallEvaFunctionByName %s, Result %f"), *InFunctionName.ToString(), Result);
	return Result;
}

double ATheOneBattleEvaluate::NativeEndTurn(ATheOneCharacterBase* InSelf)
{
	double Result = 0.0f;
	// 如果还有能执行的行动，那么得分较低
	auto RemainActionPoint = InSelf->GetAttributeSet()->GetActionPoint();
	if (FMath::IsNearlyEqual(RemainActionPoint,0.001f))
	{
		// 如果行动点为0， 得分极高
		Result = 100.0f;
	}
	else if (RemainActionPoint > 0)
	{
		Result = -1.0f;
	}
	
	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
	auto TheOneBattle = ContextSystem->Battle;
	auto OppositeCamp = ATheOneBattle::GetOppositeCamp(InSelf);
	auto OppositeCharacters = TheOneBattle->GetCharactersByCamp(OppositeCamp);
	int MinDistance = INT_MAX;
	for (auto OppositeCharacter : OppositeCharacters)
	{
		auto Distance = ContextSystem->HexGrid->GetDistance(OppositeCharacter->GetCurrentHexCoord(), InSelf->GetCurrentHexCoord());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
		}
	}
	// 距离我最近的单位距离在（A, B）范围内， 那么得分较高
	if (MinDistance >= 8 && MinDistance <= 12)
	{
		Result += 2.f;
	}
	// 如果我处于优势地形， 获得额外得分
	// Todo:

	// 如果体力值较低，获得额外得分
	if (InSelf->GetAttributeSet()->GetEnergy() < 15.f)
	{
		Result += 2.f;
	}
	
	return Result;
}

double ATheOneBattleEvaluate::NativeWaitTurn(ATheOneCharacterBase* InSelf)
{
	// 如果已经执行过了等待，等分-1
	// 如果没有能直接释放的技能， 那么得分较高
	// Todo: 暂时还没有实现等待回合的功能，先不考虑
	return -1;
}

double ATheOneBattleEvaluate::NativeRashMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, int HealthSortIndex, FVector& OutTargetLocation)
{
	// 如果处于某个敌对单位的控制区， 那么得分极低
	double Result = 0.0f;
	if (InAnyOpponentZOC)
	{
		Result = -10.0f;
	}
	
	// 向目标尽可能的移动， 如果移动后还有剩余的行动点数（最好是检查有没有能直接使用的攻击类性能）， 那么得分较高
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	// 计算移动到目标的路径和消耗
	auto PC = Cast<ATheOneHexMapPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// 找到离自己最近的目标的控制区内的一个可以到达的格子
	auto TargetCoord = InTarget->GetCurrentHexCoord();
	const auto& ControlCoords = HexGrid->GetRangeCoords(TargetCoord,1,true);

	int MinDistance = INT_MAX;
	FHCubeCoord MinDistanceCoord;
	for (auto ControlCoord : ControlCoords)
	{
		const auto& Tile = HexGrid->GetHexTile(ControlCoord);
		if (Tile.bIsBlocking || Tile.HasActor())
		{
			continue;
		}

		auto Distance = HexGrid->GetDistance(InSelf->GetCurrentHexCoord(), ControlCoord);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			MinDistanceCoord = ControlCoord;
		}
	}

	if (MinDistance < INT_MAX)
	{
		OutTargetLocation = HexGrid->HexToWorld(MinDistanceCoord);
	}
	else
	{
		OutTargetLocation = InTarget->GetActorLocation();
	}
	
	FVector OutDesiredLocation;
	float OutCost = 0.0f;
	FVector OutCanArriveLocation;
	int32 OutCanMoveCount;
	TArray<FNavPathPoint> OutPath;
	bool Find = PC->FindPath(Cast<AAIController>(InSelf->GetController()), InSelf, HexGrid->SnapToGrid(OutTargetLocation),
	             OutDesiredLocation, OutCost, OutCanArriveLocation, OutPath, OutCanMoveCount);

	if (Find)
	{
		auto CurrentActionPoint = InSelf->GetAttributeSet()->GetActionPoint();
		// GetAbilityMinCost
		auto MinCost = InSelf->GetAbilityMinCost();
		// Todo: 这里可以写的更聪明， 要大于进攻性质的最小的技能的消耗
		if (CurrentActionPoint - OutCost >= MinCost)
		{
			Result += 10.0f;
			// 如果目标的血量低，那么额外得分
			if (InTarget->GetLifeAttributeSet()->GetHealth() < 30.f)
			{
				Result += 2.0f;
			}

			// 如果有队友在目标控制区内，那么额外得分
			if (HasAnyTeammateInTargetZOC(InSelf, InTarget))
			{
				Result += 2.0f;
			}
			// 血量越低，HealthSortIndex越大，得分越高
			Result += static_cast<double>(HealthSortIndex);
		}
	}
	else
	{
		Result = -1.0f;
	}
	
	return Result;
}

double ATheOneBattleEvaluate::NativeConservativeMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, int HealthSortIndex, FVector& OutTargetLocation)
{
	// 如果处于某个敌对单位的控制区， 那么得分极低
	double Result = 0.0f;
	if (InAnyOpponentZOC)
	{
		Result = -10.0f;
	}
	auto TheOneBattle = GetWorld()->GetSubsystem<UTheOneContextSystem>()->Battle;
	const auto& SelfMemory = TheOneBattle->GetChessMemory(InSelf);
	// 如果上次行动是保守移动，那么不会再次执行
	if (SelfMemory.Num() > 0)
	{
		if (SelfMemory.Last().Choice == ETheOneAIChoice::ConservativeMove)
		{
			Result = -1.0f;
		}
		return Result;
	}
	
	// 没有好的攻击机会的情况下，移动到最靠近敌人并且非控制区的格子
	// 直接向InTarget寻路， 剔除控制区的格子, 并且处理Cost
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	// 计算移动到目标的路径和消耗
	auto PC = Cast<ATheOneHexMapPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	FVector OutDesiredLocation;
	float OutCost = 0.0f;
	FVector OutCanArriveLocation;
	int32 OutCanMoveCount;
	TArray<FNavPathPoint> OutPath;
	bool Find = PC->FindPath(Cast<AAIController>(InSelf->GetController()), InSelf, HexGrid->HexToWorld(InTarget->GetCurrentHexCoord()),
				 OutDesiredLocation, OutCost, OutCanArriveLocation, OutPath, OutCanMoveCount);


	if (Find)
	{
		// 倒序检查能到达的节点是不是在Target的控制区内
		FHCubeCoord TargetCoord = InTarget->GetCurrentHexCoord();
		auto ControlCoords = HexGrid->GetRangeCoords(TargetCoord, 1, true);
		int32 FinalMoveCount = OutCanMoveCount;
		for (int i = OutCanMoveCount - 1; i >= 0; --i)
		{
			const auto& PathPoint = OutPath[i];
			if (ControlCoords.Contains(HexGrid->WorldToHex(PathPoint.Location)))
			{
				FinalMoveCount--;
				const auto& Tile = HexGrid->GetHexTile(HexGrid->WorldToHex(PathPoint.Location));
				OutCost -= Tile.Cost;
			}
			else
			{
				break;;
			}
		}

		if (FinalMoveCount > 0 && OutPath.Num() > FinalMoveCount - 1)
		{
			Result += 5.0f;
			OutTargetLocation = OutPath[FinalMoveCount - 1].Location;
			// 在目标敌人没有发生交战的情况下，得分较高
			if (!HasAnyTeammateInTargetZOC(InSelf, InTarget))
			{
				Result += 2.0f;
			}

			// 目标血量最低时，倾向增加
			Result += static_cast<double>(HealthSortIndex);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("无安全移动范围, FinalMoveCount %d, OutPath.Num() %d, OutCost: %f"), FinalMoveCount, OutPath.Num(), OutCost);
			OutTargetLocation = OutCanArriveLocation;
			Result = -1.f;
		}
	}
	else
	{
		Result = -1.0f;
	}
	
	return Result;
}

double ATheOneBattleEvaluate::GeneralSingleMelee(const FTheOneEvaParams& InParams)
{
	const FTheOneAbilityCache* AbilityCache = InParams.Self->GetAbilityCacheByIntPayload(InParams.AbilityIndex);
	if(!AbilityCache)
	{
		return -1.f;
	}

	// 首先检查能不能把释放
	auto AbilityConfig = AbilityCache->AbilityGA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("GeneralSingleMelee");
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	bool InRange = HexGrid->InRangeByVector(InParams.Self->GetActorLocation(), InParams.Target->GetActorLocation(), AbilityConfig->ActiveAbilityData.ReleaseDistance);
	bool ActionPointEnough = InParams.Self->GetAttributeSet()->GetActionPoint() >= AbilityConfig->Cost;
	if (!InRange || !ActionPointEnough)
	{
		return -1.f;
	}

	double Result = 0.0f;

	Result += static_cast<double>(InParams.HealthSortIndex);

	// 预计能否打死对方
	if (AbilityCache->AbilityGA->Implements<UTheOneAbilityEvaInterface>())
	{
		float Damage = 0.0f;
		bool MaybeDead = false;
		ITheOneAbilityEvaInterface::Execute_PredictDamage(AbilityCache->AbilityGA.Get(), InParams.Target, Damage, MaybeDead);
		Result += Damage;
		if (MaybeDead)
		{
			Result += 10.0f;
		}
	}
	
	return Result;
}

bool ATheOneBattleEvaluate::HasAnyTeammateInTargetZOC(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
	auto Battle = ContextSystem->Battle;
	auto HexGrid = ContextSystem->HexGrid;
	auto TeamCharacters = Battle->GetCharactersByCamp(IInHexActorInterface::Execute_GetCamp(InSelf));

	auto TargetCoord = InTarget->GetCurrentHexCoord();
	const auto& ControlCoords = HexGrid->GetRangeCoords(TargetCoord,1,true);
	for (auto TeamCharacter : TeamCharacters)
	{
		if (ControlCoords.Contains(TeamCharacter->GetCurrentHexCoord()))
		{
			return true;
		}
	}

	return false;
}
