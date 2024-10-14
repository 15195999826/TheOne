// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerControllers//TheOneHexMapPlayerController.h"

#include "AIController.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "AI/TheOneAICommandInterface.h"
#include "Character/TheOneCharacterBase.h"
#include "Game/CursorTraceInterface.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"

void ATheOneHexMapPlayerController::OnCharacterGetTurn(ATheOneCharacterBase* InCharacter)
{
	if (IInHexActorInterface::Execute_GetCamp(InCharacter) == ETheOneCamp::Player)
	{
		CanPlayerControl = true;
	}
	else
	{
		CanPlayerControl = false;
	}
	IsCommanding = false;
	DesiredGoalLocation = FVector(FLT_MAX);
	ActiveCharacter = InCharacter;
	ActiveAI = Cast<AAIController>(InCharacter->GetController());
	MoveRequest.SetNavigationFilter(ActiveAI->GetDefaultNavigationFilterClass());
}

void ATheOneHexMapPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MoveRequest.SetAllowPartialPath(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsUsingPartialPaths()));
	MoveRequest.SetAcceptanceRadius(5.f);
	MoveRequest.SetCanStrafe(false);
	MoveRequest.SetReachTestIncludesAgentRadius(false);
	MoveRequest.SetReachTestIncludesGoalRadius(true);
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetRequireNavigableEndLocation(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsNavigableEndLocationRequired()));
	MoveRequest.SetProjectGoalLocation(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsProjectingGoal()));

	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCommandBehaviorEnd.AddDynamic(this, &ATheOneHexMapPlayerController::OnCommandBehaviorEnd);
	EventSystem->OnCharacterGetTurn.AddUObject(this, &ATheOneHexMapPlayerController::OnCharacterGetTurn);
}

void ATheOneHexMapPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OverWidget)
	{
		if (Focusing)
		{
			Focusing = false;
			FocusType = ETheOneFocusType::None;
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->OnLoseFocus.Broadcast();
		}
		return;
	}
	
	switch (FocusType) {
		case ETheOneFocusType::None:
			break;
		case ETheOneFocusType::Character:
		case ETheOneFocusType::Tile:
			{
				FocusTimer += DeltaSeconds;
				if (!Focusing && FocusTimer >= FocusTriggerTime)
				{
					Focusing = true;
					int TileIndex = INDEX_NONE;
					if (FocusType == ETheOneFocusType::Tile)
					{
						TileIndex = FocusCurrentCoord.IsValid()?GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid->GetHexTileIndex(FocusCurrentCoord) : INDEX_NONE;
						if (TileIndex == INDEX_NONE)
						{
							break;
						}
					}
					
					FTheOneFocusData FocusData;
					FocusData.FocusType = FocusType;
					FocusData.FocusCharacter = FocusCharacter;
					FocusData.TileIndex =  TileIndex;
					auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
					EventSystem->OnGetFocus.Broadcast(FocusData);
				}
			}
			break;
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitGround(const FVector& InHitLocation, FVector& OutGroundLocation)
{
	if (PostRealHitLocation)
	{
		OutGroundLocation = InHitLocation;
	}
	else
	{
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		const double TempZ{InHitLocation.Z};
		const FHCubeCoord& HexCoord = HexGrid->WorldToHex(InHitLocation);
		FVector Result{ HexGrid->HexToWorld(HexCoord) };
		Result.Z = TempZ;
		OutGroundLocation = Result;
		if (FocusCurrentCoord != HexCoord || FocusType != ETheOneFocusType::Tile)
		{
			Focusing = false;
			FocusType = ETheOneFocusType::Tile;
			FocusCharacter = nullptr;
			FocusCurrentCoord = HexCoord;
			FocusTimer = 0;
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->OnLoseFocus.Broadcast();
		}
	}
	
	if (bIsPawnImplementICursorTrace)
	{
		ICursorTraceInterface::Execute_OnHitGround(GetPawn(), OutGroundLocation);
	}
	
	if (auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid)
	{
		HexGrid->UpdateHitLocation(OutGroundLocation);
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitCharacter(ATheOneCharacterBase* HitCharacter)
{
	Super::GeneralOnHitCharacter(HitCharacter);
	if (FocusType != ETheOneFocusType::Character)
	{
		Focusing = false;
		FocusType = ETheOneFocusType::Character;
		FocusCharacter = HitCharacter;
		FocusCurrentCoord = FHCubeCoord::ErrorCoord;
		FocusTimer = 0;
		auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
		EventSystem->OnLoseFocus.Broadcast();
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitNone()
{
	if (Focusing)
	{
		Focusing = false;
		auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
		EventSystem->OnLoseFocus.Broadcast();
	}
	FocusType = ETheOneFocusType::None;

}

bool ATheOneHexMapPlayerController::CanWalk(const FVector& InLocation) const
{
	return GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid->IsTileReachable(InLocation);
}

void ATheOneHexMapPlayerController::ShowReleaseDistanceTips()
{
	if (ActiveCharacter.IsValid())
    {
		OnSelectedCharacterEnterNewCoord(ActiveCharacter->GetCurrentHexCoord());

		// Todo: 战棋目前看来不需要监听角色的位置变化
		// if  (!HasListenEnterNewCoord)
		// {
		// 	HasListenEnterNewCoord = true;
		// 	// 监听SelectedCharacter的位置更新
		// 	SelectedCharacter->OnEnterNewCoord.AddDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		// }
    }
}

void ATheOneHexMapPlayerController::HideReleaseDistanceTips()
{
	if (ActiveCharacter.IsValid())
	{
		// ActiveCharacter->OnEnterNewCoord.RemoveDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		// HasListenEnterNewCoord = false;
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		for (const auto& HexIndex : TipsHexCache)
		{
			HexGrid->SetWireFrameColor(HexIndex,HexGrid->WireframeDefaultColor, 0.f);
		}
		TipsHexCache.Empty();
	}
}

void ATheOneHexMapPlayerController::HideAllPathTips()
{
	for (const auto& Tip : ShowingPathTips)
	{
		ReleasePathTip(Tip);
	}
	ShowingPathTips.Empty();
}

void ATheOneHexMapPlayerController::BP_OnHitGround_Implementation(const FVector& HitLocation, bool bIsRightClick,
                                                                  bool bIsLeftClick, bool CanWalk)
{
	if (!CanPlayerControl)
	{
		return;
	}
	
	if (bIsRightClick)
	{
		HideAllPathTips();
		DesiredGoalLocation = FVector(FLT_MAX);
	}

	if (!IsCommanding && bIsLeftClick && ActiveAI.IsValid())
	{
		HideAllPathTips();
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		if (HitLocation.Equals(DesiredGoalLocation, 0.1f))
		{
			DesiredGoalLocation = FVector(FLT_MAX);

			auto Height = HexGrid->GetTileHeightByCoord(HexGrid->WorldToHex(CanArriveGoalLocation));

			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			auto Payload = FTheOneUseAbilityCommandPayload(
				ETheOneUseAbilityCommandType::Move, 0, CanArriveGoalLocation + FVector(0, 0, Height), CurrentCost);
			EventSystem->UseAbilityCommand.Broadcast(Payload);
		}
		else
		{
			DesiredGoalLocation = HitLocation;
			// 显示移动路径
			bool Set = MoveRequest.UpdateGoalLocation(HitLocation);
			if (!Set)
			{
				UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, Set GoalLocation failed"),
				       *ActiveAI->GetPawn()->GetName());
				return;
			}

			int32 OutCanMoveCount;
			TArray<FNavPathPoint> OutPath;
			bool Find = FindPath(ActiveAI.Get(), ActiveCharacter.Get(), HitLocation, DesiredGoalLocation, CurrentCost,
			                     CanArriveGoalLocation, OutPath, OutCanMoveCount);

			if (Find)
			{
				for (int i = 0; i < OutCanMoveCount; i++)
				{
					auto Height = HexGrid->GetTileHeightByCoord(HexGrid->WorldToHex(OutPath[i].Location));
					auto Tip = GetPathTip();
					ShowingPathTips.Add(Tip);
					Tip->SetActorLocation(OutPath[i].Location + FVector(0, 0, Height));
				}
			}
		}
	}
}

void ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord(const FHCubeCoord& InCoord)
{
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	for (const auto& HexIndex : TipsHexCache)
	{
		HexGrid->SetWireFrameColor(HexIndex,HexGrid->WireframeDefaultColor, 0.f);
	}
	TipsHexCache.Empty();
	auto CurrentCoord = ActiveCharacter->GetCurrentHexCoord();
	auto Hexes = HexGrid->GetRange(CurrentCoord, ReleaseDistanceCache);
	for (const auto& Hex : Hexes)
	{
		auto MeshInstanceIndex = HexGrid->GetHexTileIndex(Hex.CubeCoord);
		HexGrid->SetWireFrameColor(MeshInstanceIndex, FLinearColor::Green, 0.3f);
		TipsHexCache.Add(MeshInstanceIndex);
	}
}

AActor* ATheOneHexMapPlayerController::GetPathTip()
{
	if (PathTipPool.Num() > 0)
	{
		auto Tip = PathTipPool.Pop();
		Tip->SetActorHiddenInGame(false);
		return Tip;
	}

	auto Tip = GetWorld()->SpawnActor<AActor>(PathTipClass);
	return Tip;
}

void ATheOneHexMapPlayerController::ReleasePathTip(AActor* InTip)
{
	InTip->SetActorHiddenInGame(true);
	PathTipPool.Add(InTip);
}

bool ATheOneHexMapPlayerController::FindPath(AAIController* InAI, ATheOneCharacterBase* InCharacter,
                                             const FVector& GoalLocation, FVector& OutDesiredGoalLocation,
                                             float& OutCost, FVector& OutCanArriveGoalLocation, TArray<FNavPathPoint>& OutPath, int& OutCanMoveCount)
{
	bool Set = MoveRequest.UpdateGoalLocation(GoalLocation);
	if (!Set)
	{
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, Set GoalLocation failed"), *InAI->GetPawn()->GetName());
		return false;
	}
	// start new move request
	FPathFindingQuery PFQuery;
	const bool bValidQuery = InAI->BuildPathfindingQuery(MoveRequest, PFQuery);
	if (!bValidQuery)
	{
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, bValidQuery is not valid"), *InAI->GetPawn()->GetName());
		return false;
	}
	FNavPathSharedPtr FollowedPath;
	InAI->FindPathForMoveRequest(MoveRequest, PFQuery, FollowedPath);
	if (FollowedPath.IsValid())
	{
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		OutPath = FollowedPath->GetPathPoints();
		// 移除第一个点， 这个点时角色当前的位置
		OutPath.RemoveAt(0);
		if (OutPath.Num() > 0)
		{
			// 判断终点能不能走
			const auto& LastPoint = OutPath.Last();
			const auto& LastHex = HexGrid->GetHexTile(LastPoint.Location);
			if (LastHex.HasActor())
			{
				// 移除最后一个点
				OutPath.RemoveAt(OutPath.Num() - 1);
			}
		}
				
		if (OutPath.Num() == 0)
		{
			OutDesiredGoalLocation = FVector(FLT_MAX);
			return false;
		}
				
		// 预测移动消耗
		OutCost = 0;
		float Cost = 0;
		int CanMoveCount = 0;
		// 获取当前角色剩余的行动点数
		auto RemainActionPoint = InCharacter->GetAttributeSet()->GetActionPoint();
		for (const auto& PathPoint : OutPath)
		{
			const auto& Tile = HexGrid->GetHexTile(PathPoint.Location);
			Cost += Tile.Cost;
			if (RemainActionPoint < Cost){
				break;
			}
			OutCost += Tile.Cost;
			CanMoveCount++;
		}

		if (CanMoveCount > 0)
		{
			OutCanArriveGoalLocation = OutPath[CanMoveCount - 1].Location;
			OutCanMoveCount = CanMoveCount;
			return true;
		}
	}
	OutDesiredGoalLocation = FVector(FLT_MAX);
	return false;
}

void ATheOneHexMapPlayerController::ReceiveUseAbilityCommand(const FTheOneUseAbilityCommandPayload& Payload)
{
	HasUseAbilityCommandCache = false;
	// Todo:  检查这个道具是否需要装备在角色身上才能用， 目前没有全局使用的道具
	//  目前仅支持，1，2，3三个技能的使用
	if (!ActiveAI.IsValid())
	{
		return;
	}
	
	IsCommanding = true;
	
	HideAllPathTips();
	
	FTheOneAbilityConfig* AbilityConfig = nullptr;
	auto InUseAbilityCommandType = Payload.CommandType;
	// Todo: 根据当前技能的使用方式， 决定目标
	switch (InUseAbilityCommandType)
	{
		case ETheOneUseAbilityCommandType::Move:
			{
				// 消耗行动点数
				if (!CanPlayerControl)
				{
					// 寻找实际寻路路径和消耗
					int32 OutCanMoveCount;
					TArray<FNavPathPoint> OutPath;
					bool Find = FindPath(ActiveAI.Get(), ActiveCharacter.Get(), Payload.VectorPayload, DesiredGoalLocation, CurrentCost,
										 CanArriveGoalLocation, OutPath, OutCanMoveCount);

					if (!Find)
					{
						UE_LOG(LogTheOneAI, Error, TEXT("AI寻找了错误的终点: %s"), *Payload.VectorPayload.ToString());
						DrawDebugSphere(GetWorld(), Payload.VectorPayload, 50, 10, FColor::Red, false, 999);
						return;
					}
					
					auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
					auto Height = HexGrid->GetTileHeightByCoord(HexGrid->WorldToHex(CanArriveGoalLocation));
					ConsumeActionPoint(ActiveCharacter.Get(), CurrentCost);
					FTheOneUseAbilityCommandPayload NewPayload = Payload;
					NewPayload.FloatPayload = CurrentCost;
					NewPayload.VectorPayload = CanArriveGoalLocation + FVector(0, 0, Height);
					ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(), NewPayload, nullptr, NewPayload.VectorPayload, 0);
				}
				else
				{
					ConsumeActionPoint(ActiveCharacter.Get(), Payload.FloatPayload);
					ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(), Payload, nullptr, Payload.VectorPayload, 0);
				}
				return;
			}
		case ETheOneUseAbilityCommandType::UseItem:
			break;
		case ETheOneUseAbilityCommandType::UseAbility:
			{
				auto Ability = ActiveCharacter->GetAbilityCacheByIntPayload(Payload.IntPayload);
				check(Ability);
				AbilityConfig = Ability->AbilityGA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("ReceiveUseAbilityCommand");
			}
			break;
		case ETheOneUseAbilityCommandType::EndTurn:
			{
				ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(), Payload, nullptr, Payload.VectorPayload, 0);
				return;
			}
		case ETheOneUseAbilityCommandType::WaitTurn:
			{
				// Todo:
			}
			break;
		default:
			UE_LOG(LogTheOne, Error, TEXT("UnSolved UseAbilityCommandType: %d"), InUseAbilityCommandType);
			break;
	}

	// 检查行动点数够不够释放的
	check(AbilityConfig)
	auto ActionPoint = static_cast<int>(ActiveCharacter->GetAttributeSet()->GetActionPoint());
	auto Cost = AbilityConfig->Cost;
	if (ActionPoint < Cost)
	{
		UE_LOG(LogTheOne, Error, TEXT("ActionPoint is not enough, Current: %d, Need: %d"), ActionPoint, Cost);
		IsCommanding = false;
		auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
		EventSystem->OnAbilityCommandErrorTarget.Broadcast();
		return;
	}
	switch (AbilityConfig->ActiveAbilityData.TargetType)
	{
		case ETheOneAbilityReleaseTarget::None:
			ConsumeActionPoint(ActiveCharacter.Get(), static_cast<float>(Cost));
			ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(),
															 Payload, nullptr,
															 ActiveCharacter->GetActorLocation(),
															 AbilityConfig->ActiveAbilityData.ReleaseDistance);
			break;
		case ETheOneAbilityReleaseTarget::Enemy:
			if (CanPlayerControl)
			{
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Enemy,
								  AbilityConfig->ActiveAbilityData.ReleaseDistance);
			}
			else
			{
				ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(),
															 Payload, Payload.ActorPayload,
															 Payload.ActorPayload->GetActorLocation(),
															 AbilityConfig->ActiveAbilityData.ReleaseDistance);
			}
			break;
		case ETheOneAbilityReleaseTarget::Ally:
			HasUseAbilityCommandCache = true;
			ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Ally,
			                  AbilityConfig->ActiveAbilityData.ReleaseDistance);
			break;
		case ETheOneAbilityReleaseTarget::AnyActor:
			HasUseAbilityCommandCache = true;
		// 改变Cursor样式， 等待玩家点击
			ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Any,
			                  AbilityConfig->ActiveAbilityData.ReleaseDistance);
			break;
		case ETheOneAbilityReleaseTarget::Grid:
			HasUseAbilityCommandCache = true;
			ChangeCursorState(ETheOneCursorState::SelectGround, ETheOneSelectActorType::Any,
			                  AbilityConfig->ActiveAbilityData.ReleaseDistance);
			break;
	}

	if (HasUseAbilityCommandCache)
	{
		ReleaseDistanceCache = AbilityConfig->ActiveAbilityData.ReleaseDistance;
		PayloadCache = Payload;
		CostCache = static_cast<float>(Cost);
		ShowReleaseDistanceTips();
		auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
		EventSystem->OnAbilityCommandWaiting.Broadcast(Payload);
	}
}

void ATheOneHexMapPlayerController::OnCommandBehaviorEnd(bool bSuccess)
{
	IsCommanding = false;
}

void ATheOneHexMapPlayerController::OnAbilityCommandCanceled()
{
	HideReleaseDistanceTips();
	IsCommanding = false;
}

void ATheOneHexMapPlayerController::OnAbilityCommandFinished()
{
	HideReleaseDistanceTips();
}

bool ATheOneHexMapPlayerController::InReleaseDistance(const FVector& SourceLocation, const FVector& TargetLocation,
	int InReleaseDistance)
{
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	return HexGrid->InRangeByVector(SourceLocation, TargetLocation, InReleaseDistance);
}

void ATheOneHexMapPlayerController::ConsumeActionPoint(ATheOneCharacterBase* InCharacter, float InCost)
{
	auto ActionPoint = InCharacter->GetAttributeSet()->GetActionPoint();
	check(ActionPoint >= InCost);
	ActiveCharacter->GetAttributeSet()->SetActionPoint(ActionPoint - InCost);
}

ATheOneCharacterBase* ATheOneHexMapPlayerController::GetExecCharacter()
{
	return ActiveCharacter.Get();
}

void ATheOneHexMapPlayerController::EndActiveCharacterTurn()
{
	HideAllPathTips();
	if (!ActiveCharacter.IsValid())
	{
		UE_LOG(LogTheOne, Error, TEXT("ActiveCharacter is not valid"));
	}
}
