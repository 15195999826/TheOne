// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneBattle.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "Actor/TheOneLevelSettingActor.h"
#include "AI/TheOneAIController.h"
#include "Game/TheOneEventSystem.h"
#include "Game/TheOneGameInstance.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"
#include "HexGrid/HexGrid.h"
#include "HexGrid/HexPathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneContextSystem.h"
#include "Subsystems/TheOneTeamSystem.h"
#include "UI/TheOneUIRoot.h"


ETheOneCamp ATheOneBattle::GetOppositeCamp(ATheOneCharacterBase* Character)
{
	ETheOneCamp CurrentCamp = IInHexActorInterface::Execute_GetCamp(Character);
	return CurrentCamp == ETheOneCamp::Player ? ETheOneCamp::Enemy : ETheOneCamp::Player;
}

void ATheOneBattle::OnEnterBattle()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterEndTurn.AddUObject(this, &ATheOneBattle::OnCharacterEndTurn);
	EventSystem->NativeBeforeCharacterMove.AddDynamic(this, &ATheOneBattle::BeforeCharacterMove);
	EventSystem->NativeAfterCharacterMove.AddDynamic(this, &ATheOneBattle::AfterCharacterMove);
	
	// 拿到玩家队伍的全部成员
	auto GameMode = Cast<ATheOneGameModeBase>(UGameplayStatics::GetGameMode(this));
	
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	// Time to Create a Random Seed
	auto TimeSeed = FDateTime::Now().ToUnixTimestamp();
	HexGrid->CreateGrid(TimeSeed);

	auto TeamSystem = GetWorld()->GetSubsystem<UTheOneTeamSystem>();
	// 创建敌人队伍
	EnemyTeamID = TeamSystem->CreateTeam();
	auto CharacterTemplateTable = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable.LoadSynchronous();
	// 随机5个敌人单位
	auto RowNames = CharacterTemplateTable->GetRowNames();
	int StartIndex = 3;
	for (int i = 0; i < 5; i++)
	{
		auto RowName = RowNames[FMath::RandRange(0, RowNames.Num() - 1)];
		TeamSystem->AddCharacterToTeam(EnemyTeamID, RowName, StartIndex + i, false);
	}

	// 将角色放置到战场中
	// 玩家在左边，敌人在右边
	int PlayerStartRow = 3;
	int PlayerStartCol = -4;

	int EnemyStartRow = -PlayerStartRow;
	int EnemyStartCol = PlayerStartCol;

	const auto& PlayerTeam = TeamSystem->GetTeam(TeamSystem->GetPlayerTeamID());
	const auto& EnemyTeam = TeamSystem->GetTeam(EnemyTeamID);
	// 初始化角色状态， Todo: 目前，仅重置血量, 未来数据是持续保存的
	BattleContext = FTheOneBattleContext();
	
	TeamMoveToBattleArea(PlayerTeam, FRotator(0, 0, 0), PlayerStartRow, PlayerStartCol, GameMode, HexGrid, TeamSystem);
	TeamMoveToBattleArea(EnemyTeam, FRotator(0, 180, 0), EnemyStartRow, EnemyStartCol, GameMode, HexGrid, TeamSystem);


	BattleContext.SetStage(ETheOneBattleStage::EnterNewRound);
}

void ATheOneBattle::BattleTick()
{
	if (BattleContext.ShouldResolveNewStage())
	{
		BattleContext.ResolveStage();
		switch (BattleContext.Stage) {
			case ETheOneBattleStage::None:
				break;
			case ETheOneBattleStage::EnterNewRound:
				NextRound();
				break;
			case ETheOneBattleStage::NewRoundPending:
				{
					WaitSignalChangeTo(ETheOneBattleStage::EnterCharacterTurn);
					// 进行一系列的表演，目前只在UI上显示一段时间回合数
					OnBattleStageChanged();
				}
				break;
			case ETheOneBattleStage::EnterCharacterTurn:
				NextCharacterTurn();
				break;
			case ETheOneBattleStage::CharacterTurn:
				WaitSignalChangeTo(ETheOneBattleStage::EnterCharacterTurn);
				OnBattleStageChanged();
				break;
		}
	}
}

void ATheOneBattle::CompleteWaitSignal() const
{
	WaitSignal.Execute();
}

UTheOneBattleWindow* ATheOneBattle::GetBattleWindow() const
{
	return Cast<UTheOneGameInstance>(UGameplayStatics::GetGameInstance(this))->UIRoot->BattleWindow;
}

void ATheOneBattle::BeforeCharacterMove(ATheOneCharacterBase* InCharacter)
{
	// 如果在某个单位的控制区内， 则抛出离开控制区事件
	const auto& CurrentCoord = InCharacter->GetCurrentHexCoord();
	// DrawDebugSphere(GetWorld(), GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid->HexToWorld(CurrentCoord), 100, 12, FColor::Green, false, 1.f);
	const auto& HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	auto OppositeCamp = GetOppositeCamp(InCharacter);
	TArray<ATheOneCharacterBase*> LevelZOCCharacters;
	for (const auto& Pair:InBattleCharacters)
	{
		if (ITheOneBattleInterface::Execute_IsDead(Pair.Value.Get()))
		{
			continue;
		}
		
		if (IInHexActorInterface::Execute_GetCamp(Pair.Value.Get()) == OppositeCamp)
		{
			const auto& Coord = Pair.Value->GetCurrentHexCoord();
			const auto& Coords = HexGrid->GetRangeCoords(Coord,1,true);
			// 检查当前格子在不在范围内
			for (const auto& InCoord: Coords)
			{
				auto Location = HexGrid->HexToWorld(InCoord);
				// DrawDebugSphere(GetWorld(), Location, 100, 12, FColor::Red, false, 1.f);
				if (InCoord == CurrentCoord)
				{
					// 触发离开控制区事件
					LevelZOCCharacters.Add(Pair.Value.Get());
					break;
				}
			}
		}
	}

	FGameplayEventData Payload;
	Payload.OptionalObject = InCharacter;
	for (auto Character : LevelZOCCharacters)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, TheOneGameplayTags::Battle_Event_LeaveZOC, Payload);
	}
}

void ATheOneBattle::AfterCharacterMove(ATheOneCharacterBase* InCharacter)
{
	// 更新控制区
	
}

void ATheOneBattle::OnBattleStageChanged_Implementation()
{
	switch (BattleContext.Stage) {
		case ETheOneBattleStage::None:
			break;
		case ETheOneBattleStage::EnterNewRound:
			break;
		case ETheOneBattleStage::NewRoundPending:
			break;
		case ETheOneBattleStage::EnterCharacterTurn:
			break;
		case ETheOneBattleStage::CharacterTurn:
			{
				auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
				if (ITheOneBattleInterface::Execute_IsDead(CurrentCharacter))
				{
					CompleteWaitSignal();
					return;
				}
				
				auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
				
				EventSystem->OnCharacterGetTurn.Broadcast(CurrentCharacter);
				// 等待玩家操作结束回合
			}
			break;
	}
}

void ATheOneBattle::TeamMoveToBattleArea(const TArray<uint32>& Team, FRotator InRotation, int InStartRow, int InStartCol,
                                         ATheOneGameModeBase* GameMode, AHexGrid* HexGrid, UTheOneTeamSystem* TeamSystem)
{
	for (int i = 0; i < Team.Num(); i++)
	{
		auto Flag = Team[i];
		BattleContext.ChessContextMap.Add(Flag, FTheOneChessContext());
		const auto& CharacterUnique = TeamSystem->GetCharacterUnique(Flag);
		auto Ctrl = GameMode->SpawnedAIMap[CharacterUnique.Flag];
		auto Character = Ctrl->GetPawn();
		InBattleCharacters.Add(Flag, CastChecked<ATheOneCharacterBase>(Character));
		auto RowOffset = CharacterUnique.TeamPosition / 8;
		auto ColOffset = CharacterUnique.TeamPosition % 8;
		auto Row = InStartRow + RowOffset;
		auto Col = InStartCol + ColOffset;
		const auto& Coord = HexGrid->GetHexCoordByXY(Row, Col);
		// Todo: 如果当前Coord是bBlocking的， 需要找到一个最近的非bBlocking的Coord

		const float Height = HexGrid->GetTileHeightByCoord(Coord);
		Character->SetActorLocation(HexGrid->HexToWorld(Coord) + FVector(0, 0, Height));
		Character->SetActorRotation(InRotation);
		// 更新PathFollowingCom的数据
		if (auto HexPathFollow = Cast<UHexPathFollowingComponent>(Ctrl->GetPathFollowingComponent()))
		{
			HexPathFollow->SetCoord(Coord);
		}

		// Cast To ITheOneBattleInterface
		auto BattleInterface = Cast<ITheOneBattleInterface>(Character);
		check(BattleInterface)
		BattleInterface->BeforeEnterBattle();
	}
}

void ATheOneBattle::WaitSignalChangeTo(ETheOneBattleStage NewStage)
{
	check(!WaitSignal.IsBound())

	WaitSignal.BindLambda([this, NewStage]()
	{
		BattleContext.SetStage(NewStage);
		WaitSignal.Unbind();
	});
}

void ATheOneBattle::NextRound()
{
	BattleContext.Round++;
	BattleContext.CurrentTurn = -1;
	// 回合阶段: 1. 回合开始 2.确定行动数据 3.每个角色进行行动 4.回合结束
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnRoundBegin.Broadcast(BattleContext.Round);
	// 计算行动顺序
	BattleContext.ActionQueue.Empty();
	// Todo: 根据速度计算， 目前直接先我方后敌方
	auto TeamSystem = GetWorld()->GetSubsystem<UTheOneTeamSystem>();
	const auto& PlayerTeam = TeamSystem->GetTeam(TeamSystem->GetPlayerTeamID());
	const auto& EnemyTeam = TeamSystem->GetTeam(EnemyTeamID);
	for (auto Flag : PlayerTeam)
	{
		auto Character = InBattleCharacters[Flag].Get();
		if (ITheOneBattleInterface::Execute_IsDead(Character))
		{
			continue;
		}
		BattleContext.ActionQueue.Add(Character);
	}

	for (auto Flag : EnemyTeam)
	{
		auto Character = InBattleCharacters[Flag].Get();
		if (ITheOneBattleInterface::Execute_IsDead(Character))
		{
			continue;
		}
	
		BattleContext.ActionQueue.Add(Character);
	}

	// 根据速度Sort
	BattleContext.ActionQueue.Sort([](const ATheOneCharacterBase& A, const ATheOneCharacterBase& B)
	{
		float SpeedA = A.GetAttributeSet()->GetSpeed();
		float SpeedB = B.GetAttributeSet()->GetSpeed();
		return SpeedA > SpeedB;
	});

	// 打印行动顺序
	for (int i = 0; i < BattleContext.ActionQueue.Num(); i++)
	{
		auto Character = BattleContext.ActionQueue[i];
		UE_LOG(LogTheOne, Log, TEXT("Round: %d, Character: %s, Speed: %f, Index: %d"), BattleContext.Round,
		       *Character->GetName(), Character->GetAttributeSet()->GetSpeed(), i);
	}
	
	// 所有角色行动点数恢复到最大值
	for (auto const& Character : BattleContext.ActionQueue)
	{
		auto Attr = Character->GetAttributeSet();
		auto MaxActionPoint = Attr->GetMaxActionPoint();
		Attr->SetActionPoint(MaxActionPoint);
	}
	
	BattleContext.SetStage(ETheOneBattleStage::NewRoundPending);
}

void ATheOneBattle::NextCharacterTurn()
{
	BattleContext.CurrentTurn++;
	if (BattleContext.CurrentTurn >= BattleContext.ActionQueue.Num())
	{
		BattleContext.SetStage(ETheOneBattleStage::EnterNewRound);
		return;
	}
	// 更新HexGridTile的寻路消耗, 如果该格子被敌方占据，寻路消耗增加
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	// 更新所有Tile的控制区为false
	for (int i = 0; i < HexGrid->GridTiles.Num(); i++)
	{
		auto& Tile = HexGrid->GridTiles[i];
		Tile.bOpponentControl = false;
		HexGrid->SetFaceIndicatorColor(i, HexGrid->FaceIndicatorDefaultColor, 0.f);
	}

	auto Character = BattleContext.ActionQueue[BattleContext.CurrentTurn];
	ETheOneCamp OppositeCamp = GetOppositeCamp(Character);
	
	// 遍历所有角色，标记敌方控制区
	for (const auto& Pair:InBattleCharacters)
	{
		if (ITheOneBattleInterface::Execute_IsDead(Pair.Value.Get()))
		{
			continue;
		}
		
		if (IInHexActorInterface::Execute_GetCamp(Pair.Value.Get()) == OppositeCamp)
		{
			// 周围6格设置为敌方控制区
			const auto& Coord = Pair.Value->GetCurrentHexCoord();
			const auto& Coords = HexGrid->GetRangeCoords(Coord,1,true);
			
			
			// 打印Len
			UE_LOG(LogTheOne, Log, TEXT("Opponent Control Len: %d"), Coords.Num());
			
			for (const auto& InCoord: Coords)
			{
				UE_LOG(LogTheOne, Log, TEXT("Opponent Control: %s"), *InCoord.ToString());
				auto& MutableTile = HexGrid->GetMutableHexTile(InCoord);
				MutableTile.bOpponentControl = true;
			}
		}
	}

	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
	for (int i = 0; i < HexGrid->GridTiles.Num(); i++)
	{
		const auto& Tile = HexGrid->GridTiles[i];
		if (Tile.bOpponentControl)
		{
			HexGrid->SetFaceIndicatorColor(i, ContextSystem->LevelSetting->OppnentCampZOCColor, 0.f);
		}
	}
	
	BattleContext.SetStage(ETheOneBattleStage::CharacterTurn);
	// 1. 行动角色， 现实一个箭头UI
	// 2. 左键点击一下地面，显示移动路径， 再次点击该位置，进行移动
}

void ATheOneBattle::OnCharacterEndTurn(ATheOneCharacterBase* InCharacter)
{
	auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
	check(CurrentCharacter == InCharacter)
	CompleteWaitSignal();
}
