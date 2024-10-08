// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneBattle.h"

#include "AbilitySystem/TheOneAttributeSet.h"
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

void ATheOneBattle::OnEnterBattle()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterEndTurn.AddUObject(this, &ATheOneBattle::OnCharacterEndTurn);
	
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

	TeamMoveToBattleArea(PlayerTeam, FRotator(0, 0, 0), PlayerStartRow, PlayerStartCol, GameMode, HexGrid, TeamSystem);
	TeamMoveToBattleArea(EnemyTeam, FRotator(0, 180, 0), EnemyStartRow, EnemyStartCol, GameMode, HexGrid, TeamSystem);

	// 初始化角色状态， Todo: 目前，仅重置血量, 未来数据是持续保存的
	BattleContext = FTheOneBattleContext();
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
				auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
				auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
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
		const auto& CharacterUnique = TeamSystem->GetCharacterUnique(Flag);
		auto Ctrl = GameMode->SpawnedAIMap[CharacterUnique.Flag];
		auto Character = Ctrl->GetPawn();
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
	auto GameMode = Cast<ATheOneGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (auto Flag : PlayerTeam)
	{
		auto Character = Cast<ATheOneCharacterBase>(GameMode->SpawnedAIMap[Flag]->GetPawn());
		BattleContext.ActionQueue.Add(Character);
	}

	for (auto Flag : EnemyTeam)
	{
		auto Character = Cast<ATheOneCharacterBase>(GameMode->SpawnedAIMap[Flag]->GetPawn());
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
