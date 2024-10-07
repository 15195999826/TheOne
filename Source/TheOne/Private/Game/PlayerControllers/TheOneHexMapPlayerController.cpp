// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerControllers//TheOneHexMapPlayerController.h"

#include "AIController.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "AI/TheOneAICommandInterface.h"
#include "Character/TheOneCharacterBase.h"
#include "Game/CursorTraceInterface.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"

void ATheOneHexMapPlayerController::ChangeActiveCharacter(ATheOneCharacterBase* InCharacter)
{
	IsCommanding = false;
	ActiveCharacter = InCharacter;
	ActiveAI = Cast<AAIController>(InCharacter->GetController());
	MoveRequest.SetNavigationFilter(ActiveAI->GetDefaultNavigationFilterClass());
}

void ATheOneHexMapPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CurrentGoalLocation = FVector(FLT_MAX);
	
	MoveRequest.SetAllowPartialPath(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsUsingPartialPaths()));
	MoveRequest.SetAcceptanceRadius(5.f);
	MoveRequest.SetCanStrafe(false);
	MoveRequest.SetReachTestIncludesAgentRadius(false);
	MoveRequest.SetReachTestIncludesGoalRadius(true);
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetRequireNavigableEndLocation(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsNavigableEndLocationRequired()));
	MoveRequest.SetProjectGoalLocation(FAISystem::PickAIOption(EAIOptionFlag::Enable, MoveRequest.IsProjectingGoal()));
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
	if (SelectedCharacter)
    {
		OnSelectedCharacterEnterNewCoord(SelectedCharacter->GetCurrentHexCoord());
		if  (!HasListenEnterNewCoord)
		{
			HasListenEnterNewCoord = true;
			// 监听SelectedCharacter的位置更新
			SelectedCharacter->OnEnterNewCoord.AddDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		}
    }
}

void ATheOneHexMapPlayerController::HideReleaseDistanceTips()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->OnEnterNewCoord.RemoveDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		HasListenEnterNewCoord = false;
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		for (const auto& HexIndex : TipsHexCache)
		{
			HexGrid->SetWireFrameColor(HexIndex,HexGrid->WireframeDefaultColor, 0.f);
		}
		TipsHexCache.Empty();
	}
}

void ATheOneHexMapPlayerController::BP_OnHitGround_Implementation(const FVector& HitLocation, bool bIsRightClick,
	bool bIsLeftClick, bool CanWalk)
{
	if (!IsCommanding && bIsLeftClick && ActiveAI.IsValid())
	{
		for (const auto& Tip : ShowingPathTips)
		{
			ReleasePathTip(Tip);
		}
		ShowingPathTips.Empty();
		if (HitLocation.Equals(CurrentGoalLocation, 0.1f))
		{
			// 移动到目标位置
			IsCommanding = true;
			auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
			auto Height = HexGrid->GetTileHeightByCoord(HexGrid->WorldToHex(CurrentGoalLocation));
			
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->UseAbilityCommand.Broadcast(FTheOneUseAbilityCommandPayload(ETheOneUseAbilityCommandType::Move, 0, CurrentGoalLocation + FVector(0, 0, Height)));
		}
		else
		{
			CurrentGoalLocation = HitLocation;
			// 显示移动路径
			bool Set = MoveRequest.UpdateGoalLocation(HitLocation);
			if (!Set)
			{
				UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, Set GoalLocation failed"), *ActiveAI->GetPawn()->GetName());
				return;
			}
		
			// start new move request
			FPathFindingQuery PFQuery;
			const bool bValidQuery = ActiveAI->BuildPathfindingQuery(MoveRequest, PFQuery);
			if (!bValidQuery)
			{
				UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, bValidQuery is not valid"), *ActiveAI->GetPawn()->GetName());
				return;
			}
			FNavPathSharedPtr FollowedPath;
			ActiveAI->FindPathForMoveRequest(MoveRequest, PFQuery, FollowedPath);
			if (FollowedPath.IsValid())
			{
				auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
				auto Path = FollowedPath->GetPathPoints();
				// 移除第一个点， 这个点时角色当前的位置
				Path.RemoveAt(0);
				for (const auto& PathPoint : Path)
				{
					auto Height = HexGrid->GetTileHeightByCoord(HexGrid->WorldToHex(PathPoint.Location));
					auto Tip = GetPathTip();
					ShowingPathTips.Add(Tip);
					Tip->SetActorLocation(PathPoint.Location+FVector(0,0,Height));
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
	auto CurrentCoord = SelectedCharacter->GetCurrentHexCoord();
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

void ATheOneHexMapPlayerController::ReceiveUseAbilityCommand(const FTheOneUseAbilityCommandPayload& Payload)
{
	HasUseAbilityCommandCache = false;
	// Todo:  检查这个道具是否需要装备在角色身上才能用， 目前没有全局使用的道具
	//  目前仅支持，1，2，3三个技能的使用
	if (!ActiveAI.IsValid())
	{
		return;
	}

	FTheOneAbilityConfig* AbilityConfig = nullptr;
	auto InUseAbilityCommandType = Payload.CommandType;
	// Todo: 根据当前技能的使用方式， 决定目标
	switch (InUseAbilityCommandType)
	{
		case ETheOneUseAbilityCommandType::Move:
			{
				// auto AI = ;
				ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(), Payload, nullptr, Payload.VectorPayload, 0);
				return;
			}
		case ETheOneUseAbilityCommandType::UseItem:
			break;
		case ETheOneUseAbilityCommandType::UseWeaponAbility:
			{
				UTheOneGeneralGA* Ability = nullptr;

				// Todo: 技能释放
				// if (IntPayload == 1)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityA.Get();
				// }
				// else if (IntPayload == 2)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityB.Get();
				// }
				// else if (IntPayload == 3)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityC.Get();
				// }

				// 因为会默认给与空技能，因此不存在空指针的情况
				// check(Ability);

				// AbilityConfig = Ability->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("ReceiveUseAbilityCommand");
			}
			break;
		default:
			UE_LOG(LogTheOne, Error, TEXT("UnSolved UseAbilityCommandType: %d"), InUseAbilityCommandType);
			break;
	}


	if (AbilityConfig)
	{
		switch (AbilityConfig->TargetType)
		{
			case ETheOneAbilityReleaseTarget::None:
				ITheOneAICommandInterface::Execute_CommitAbility(ActiveAI.Get(),
				                                                 Payload, nullptr,
				                                                 ActiveCharacter->GetActorLocation(),
				                                                 AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Enemy:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Enemy,
				                  AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Ally:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Ally,
				                  AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::AnyActor:
				HasUseAbilityCommandCache = true;
			// 改变Cursor样式， 等待玩家点击
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Any,
				                  AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Grid:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectGround, ETheOneSelectActorType::Any,
				                  AbilityConfig->ReleaseDistance);
				break;
		}

		if (HasUseAbilityCommandCache)
		{
			ReleaseDistanceCache = AbilityConfig->ReleaseDistance;
			PayloadCache = Payload;
			ShowReleaseDistanceTips();
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->OnAbilityCommandWaiting.Broadcast(Payload);
		}
	}
}
