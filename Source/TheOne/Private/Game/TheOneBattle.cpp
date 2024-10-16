// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneBattle.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "Actor/TheOneLevelSettingActor.h"
#include "AI/TheOneAIController.h"
#include "Game/TheOneEventSystem.h"
#include "Game/TheOneGameInstance.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/Battle/TheOneBattleEvaluate.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"
#include "HexGrid/HexGrid.h"
#include "HexGrid/HexPathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneContextSystem.h"
#include "Subsystems/TheOneTeamSystem.h"
#include "UI/TheOneUIRoot.h"


FString FTheOneAIChoice::ToString() const
{
	FString Ret;
	Ret.Append("{");
	Ret.Append(ChoiceNameMap[Choice]);
	Ret.Append(TEXT(", 得分:"));
	Ret.Append(FString::SanitizeFloat(Score));
	if (TargetCharacter)
	{
		Ret.Append(TEXT(", 目标角色:"));
		Ret.Append(TargetCharacter->GetName());
	}

	switch (Choice) {
		case ETheOneAIChoice::EndTurn:
			break;
		case ETheOneAIChoice::WaitTurn:
			break;
		case ETheOneAIChoice::RashMove:
			break;
		case ETheOneAIChoice::EscapeMove:
			break;
		case ETheOneAIChoice::ConservativeMove:
			break;
		case ETheOneAIChoice::ReleaseAbility:
			{
				Ret.Append(TEXT(", 技能:"));
				Ret.Append(FString::FromInt(AbilityIndex));
			}
		break;
	}
		
	Ret.Append("}");
	return Ret;
}

ETheOneCamp ATheOneBattle::GetOppositeCamp(ATheOneCharacterBase* Character)
{
	ETheOneCamp CurrentCamp = IInHexActorInterface::Execute_GetCamp(Character);
	return CurrentCamp == ETheOneCamp::Player ? ETheOneCamp::Enemy : ETheOneCamp::Player;
}

void ATheOneBattle::OnEnterBattle()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterEndTurn.AddDynamic(this, &ATheOneBattle::OnCharacterEndTurn);
	EventSystem->NativeBeforeCharacterMove.AddDynamic(this, &ATheOneBattle::BeforeCharacterMove);
	EventSystem->NativeAfterCharacterMove.AddDynamic(this, &ATheOneBattle::AfterCharacterMove);
	EventSystem->OnCommandBehaviorEnd.AddDynamic(this, &ATheOneBattle::OnCommandBehaviorEnd);
	EventSystem->OnCharacterDead.AddDynamic(this, &ATheOneBattle::OnCharacterDead);
	// Todo: 监听单位死亡，更新控制区
	
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

	// Todo: 临时给与所有人20点头部护甲和20点身体护甲, 
	for (const auto& Pair:InBattleCharacters)
	{
		auto Character = Pair.Value.Get();
		auto LifeAttributeSet = Character->GetLifeAttributeSet();
		LifeAttributeSet->SetMaxHeadArmor(20.f);
		LifeAttributeSet->SetHeadArmor(20.f);
		LifeAttributeSet->SetMaxBodyArmor(20.f);
		LifeAttributeSet->SetBodyArmor(20.f);
		auto Attribute = Character->GetAttributeSet();
		// 20~25随机最小攻击力， 25~30随机最大攻击力
		Attribute->SetMinAttack(FMath::RandRange(20.f, 25.f));
		Attribute->SetMaxAttack(FMath::RandRange(25.f, 30.f));
		Attribute->SetMaxEnergy(100.f);
		Attribute->SetEnergy(100.f);
		// 从0~100随机赋予MeleeLevel、RangedLevel、MeleeDefense、RangedDefense、DamageArmorEfficiency、DamagePenetrationEfficiency
		Attribute->SetMeleeLevel(FMath::RandRange(0, 100));
		Attribute->SetRangedLevel(FMath::RandRange(0, 100));
		Attribute->SetMeleeDefense(FMath::RandRange(0, 100));
		Attribute->SetRangedDefense(FMath::RandRange(0, 100));
		Attribute->SetDamageArmorEfficiency(FMath::RandRange(0.f, 1.f));
		Attribute->SetDamagePenetrationEfficiency(FMath::RandRange(0.f, 1.f));
	}

	BattleContext.SetStage(ETheOneBattleStage::EnterNewRound);
}

void ATheOneBattle::BattleTick()
{
	if (BattleContext.WantToGameOver && BattleContext.Stage != ETheOneBattleStage::GameOver)
	{
		BattleContext.SetStage(ETheOneBattleStage::GameOver);
	}
	
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
			case ETheOneBattleStage::GameOver:
				OnBattleStageChanged();
				break;
		}
	}

	if (BattleContext.Stage == ETheOneBattleStage::CharacterTurn && BattleContext.WantToNextAIThink)
	{
		BattleContext.WantToNextAIThink = false;
		auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, CurrentCharacter]()
		{
			AIThink(CurrentCharacter);
		});
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
				// auto Location = HexGrid->HexToWorld(InCoord);
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

				// 如果是敌人，等待AI操作完毕
				if (IInHexActorInterface::Execute_GetCamp(CurrentCharacter) == ETheOneCamp::Enemy)
				{
					BattleContext.ChessContextMap[CurrentCharacter->GetFlag()].Memory.Empty();
					AIThink(CurrentCharacter);
				}
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

void ATheOneBattle::UpdateControlTile()
{
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

		// Todo: 如果手握远程武器，那么无控制区
		
		if (IInHexActorInterface::Execute_GetCamp(Pair.Value.Get()) == OppositeCamp)
		{
			// 周围6格设置为敌方控制区
			const auto& Coord = Pair.Value->GetCurrentHexCoord();
			const auto& Coords = HexGrid->GetRangeCoords(Coord,1,true);
			
			
			// 打印Len
			// UE_LOG(LogTheOne, Log, TEXT("Opponent Control Len: %d"), Coords.Num());
			
			for (const auto& InCoord: Coords)
			{
				// UE_LOG(LogTheOne, Log, TEXT("Opponent Control: %s"), *InCoord.ToString());
				auto& MutableTile = HexGrid->GetMutableHexTile(InCoord);
				MutableTile.bOpponentControl = true;
			}
		}
	}

	auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
	if (IInHexActorInterface::Execute_GetCamp(CurrentCharacter) == ETheOneCamp::Player)
	{
		auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
		for (int i = 0; i < HexGrid->GridTiles.Num(); i++)
		{
			const auto& Tile = HexGrid->GridTiles[i];
			if (Tile.bOpponentControl)
			{
				HexGrid->SetFaceIndicatorColor(i, ContextSystem->LevelSetting->OppnentCampZOCColor, 0.f);
			}
		}
	}
}

void ATheOneBattle::NextCharacterTurn()
{
	BattleContext.CurrentTurn++;
	if (BattleContext.CurrentTurn >= BattleContext.ActionQueue.Num())
	{
		BattleContext.SetStage(ETheOneBattleStage::EnterNewRound);
		return;
	}
	
	UpdateControlTile();
	
	
	BattleContext.SetStage(ETheOneBattleStage::CharacterTurn);
	// 1. 行动角色， 现实一个箭头UI
	// 2. 左键点击一下地面，显示移动路径， 再次点击该位置，进行移动
}

void ATheOneBattle::AIThink(ATheOneCharacterBase* InCharacter)
{
	TArray<FTheOneAIChoice> PossibleChoices;
	auto PlayerTeam = GetCharactersByCamp(ETheOneCamp::Player);
	// 目标选择： 最近的目标， 远程

	// 记录血量排序，从大到小
	TMap<uint32, float> HealthMap;

	for (auto PlayerCharacter : PlayerTeam)
	{
		auto Health = PlayerCharacter->GetLifeAttributeSet()->GetHealth();
		HealthMap.Add(PlayerCharacter->GetFlag(), Health);
	}

	// 按血量高低
	TArray<uint32> HealthSort;
	HealthMap.GenerateKeyArray(HealthSort);
	HealthSort.Sort([&HealthMap](uint32 A, uint32 B)
	{
		return HealthMap[A] > HealthMap[B];
	});

	bool InAnyOpponentZOC = false;
	auto CurrentCoord = InCharacter->GetCurrentHexCoord();;
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	for (const auto& PlayerCharacter : PlayerTeam)
	{
		if (ITheOneBattleInterface::Execute_IsDead(PlayerCharacter))
		{
			continue;
		}

		const auto& Coord = PlayerCharacter->GetCurrentHexCoord();
		const auto& Coords = HexGrid->GetRangeCoords(Coord, 1, true);
		// 检查当前格子在不在范围内
		for (const auto& InCoord : Coords)
		{
			// auto Location = HexGrid->HexToWorld(InCoord);
			// DrawDebugSphere(GetWorld(), Location, 100, 12, FColor::Red, false, 1.f);
			if (InCoord == CurrentCoord)
			{
				// 触发离开控制区事件
				InAnyOpponentZOC = true;
				break;
			}
		}

		if (InAnyOpponentZOC)
		{
			break;
		}
	}

	// 遍历所有AI可以执行的行为， 计算得分
	auto BattleEva = GetWorld()->GetSubsystem<UTheOneContextSystem>()->BattleEvaluate;
	FTheOneAIChoice EndTurnChoice;
	EndTurnChoice.Choice = ETheOneAIChoice::EndTurn;
	EndTurnChoice.Score = BattleEva->NativeEndTurn(InCharacter);
	PossibleChoices.Add(EndTurnChoice);

	FTheOneAIChoice WaitTurnChoice;
	WaitTurnChoice.Choice = ETheOneAIChoice::WaitTurn;
	WaitTurnChoice.Score = BattleEva->NativeWaitTurn(InCharacter);
	PossibleChoices.Add(WaitTurnChoice);

	// 目标+得分构成一个选项, 命中仍然通过得分来角色
	// 需要选择目标的技能，通过这种方式来做
	// 尽可能优先选择能释放的技能


	auto Abilities = InCharacter->AbilityCaches;
	int32 AbilityIndex = 0;
	for (const auto& Pair : Abilities)
	{
		const auto& AbilityCacheArray = Pair.Value;
		for (const auto& AbilityCache : AbilityCacheArray)
		{
			auto AbilityRow = AbilityCache.AbilityGA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("AIEva");
			if (AbilityRow->AbilityType == ETheOneAbilityType::Active)
			{
				// Todo: 如果技能是向地面释放的
				switch (AbilityRow->ActiveAbilityData.TargetType)
				{
					case ETheOneAbilityReleaseTarget::None:
						break;
					case ETheOneAbilityReleaseTarget::Enemy:
						{
							for (auto PlayerCharacter : PlayerTeam)
							{
								if (ITheOneBattleInterface::Execute_IsDead(PlayerCharacter))
								{
									continue;
								}

								FTheOneAIChoice Choice;
								Choice.Choice = ETheOneAIChoice::ReleaseAbility;
								Choice.TargetCharacter = PlayerCharacter;
								Choice.TargetLocation = FVector(FLT_MAX);
								Choice.AbilityIndex = AbilityIndex;

								FTheOneEvaParams Params;
								Params.Self = InCharacter;
								Params.Target = PlayerCharacter;
								Params.TargetLocation = Choice.TargetLocation;
								Params.SelfInAnyOpponentZOC = InAnyOpponentZOC;
								Params.HealthSortIndex = HealthSort.IndexOfByKey(PlayerCharacter->GetFlag());
								Params.AbilityIndex = AbilityIndex;
								Choice.Score = BattleEva->CallEvaFunctionByName(
									AbilityRow->EvaluateFunctionName, Params);
								PossibleChoices.Add(Choice);
							}
						}
						break;
					case ETheOneAbilityReleaseTarget::Ally:
						break;
					case ETheOneAbilityReleaseTarget::Grid:
						break;
					case ETheOneAbilityReleaseTarget::AnyActor:
						break;
				}
			}
			AbilityIndex++;
		}
	}

	// 之后考虑两种移动方式
	for (auto PlayerCharacter : PlayerTeam)
	{
		if (ITheOneBattleInterface::Execute_IsDead(PlayerCharacter))
		{
			continue;
		}


		FTheOneAIChoice RashMoveChoice;
		RashMoveChoice.Choice = ETheOneAIChoice::RashMove;
		RashMoveChoice.TargetCharacter = PlayerCharacter;
		RashMoveChoice.Score = BattleEva->NativeRashMove(InCharacter, PlayerCharacter, InAnyOpponentZOC,
		                                                 HealthSort.IndexOfByKey(PlayerCharacter->GetFlag()),
		                                                 RashMoveChoice.TargetLocation);
		PossibleChoices.Add(RashMoveChoice);

		FTheOneAIChoice ConservativeMoveChoice;
		ConservativeMoveChoice.Choice = ETheOneAIChoice::ConservativeMove;
		ConservativeMoveChoice.TargetCharacter = PlayerCharacter;
		ConservativeMoveChoice.Score = BattleEva->NativeConservativeMove(
			InCharacter, PlayerCharacter, InAnyOpponentZOC, HealthSort.IndexOfByKey(PlayerCharacter->GetFlag()),
			ConservativeMoveChoice.TargetLocation);
		PossibleChoices.Add(ConservativeMoveChoice);
	}


	// 检查是否存在得分大于0的选项，如果一个都没有，则直接加入结束行动Choice
	bool NoPositiveChoice = true;
	for (const auto& Choice : PossibleChoices)
	{
		if (Choice.Score > 0)
		{
			NoPositiveChoice = false;
			break;
		}
	}
	
	if (NoPositiveChoice)
	{
		if (DebugDetail)
		{
			UE_LOG(LogTheOne, Log, TEXT("No Positive Choice, PrintCurrent Choice, Total: %d"), PossibleChoices.Num());
			for (const auto& Choice : PossibleChoices)
			{
				UE_LOG(LogTheOne, Log, TEXT("Choice: %s"), *Choice.ToString());
			}
		}

		PossibleChoices.Empty();
		EndTurnChoice.Score = 100.f;
		PossibleChoices.Add(EndTurnChoice);
	}

	check(PossibleChoices.Num() > 0)
	int32 ChoiceIndex = 0;
	if (PossibleChoices.Num() > 1)
	{
		if (DebugDetail)
		{
			UE_LOG(LogTheOne, Log, TEXT("筛选Choice, Total: %d"), PossibleChoices.Num());
			for (const auto& Choice : PossibleChoices)
			{
				UE_LOG(LogTheOne, Log, TEXT("Choice: %s"), *Choice.ToString());
			}
		}
		// 剔除其中得分相对小， 几乎不应该被选择的选项
		// Todo: 目前小于最高得分*20%的选项都被剔除
		double MaxScore = 0;
		for (const auto& Choice : PossibleChoices)
		{
			if (Choice.Score > MaxScore)
			{
				MaxScore = Choice.Score;
			}
		}

		// 剔除
		for (int i = PossibleChoices.Num() - 1; i >= 0; i--)
		{
			if (PossibleChoices[i].Score < MaxScore * 0.2)
			{
				if (DebugDetail)
				{
					UE_LOG(LogTheOne, Log, TEXT("Remove Choice: %s"), *PossibleChoices[i].ToString());
				}
				PossibleChoices.RemoveAt(i);
			}
		}

		check(PossibleChoices.Num() > 0)
		double TotalScore = 0.0;
		for (const auto& Choice : PossibleChoices)
		{
			TotalScore += Choice.Score;
		}

		// 每一个选项的概率为 Choice.Score / TotalScore， 随机选择一个
		double RandomValue = FMath::FRandRange(0, TotalScore);
		double AccumulateScore = 0.0;
		for (int32 i = 0; i < PossibleChoices.Num(); i++)
		{
			AccumulateScore += PossibleChoices[i].Score;
			if (RandomValue <= AccumulateScore)
			{
				ChoiceIndex = i;
				break;
			}
		}
	}

	const auto& FinalChoice = PossibleChoices[ChoiceIndex];
	UE_LOG(LogTheOne, Error, TEXT("最终选择了: %s"), *FinalChoice.ToString());
	BattleContext.ChessContextMap[InCharacter->GetFlag()].Memory.Add(FinalChoice);
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	FTheOneUseAbilityCommandPayload Payload;
	
	// 怎么执行选项呢
	switch (FinalChoice.Choice)
	{
		case ETheOneAIChoice::EndTurn:
			{
				Payload.CommandType = ETheOneUseAbilityCommandType::EndTurn;
			}
			break;
		case ETheOneAIChoice::WaitTurn:
			{
				Payload.CommandType = ETheOneUseAbilityCommandType::EndTurn;
			}
			break;
		case ETheOneAIChoice::RashMove:
		case ETheOneAIChoice::ConservativeMove:
			{
				Payload.CommandType = ETheOneUseAbilityCommandType::Move;
				Payload.VectorPayload = FinalChoice.TargetLocation;
			}
			break;
		case ETheOneAIChoice::EscapeMove:
			// Todo:
			break;
		case ETheOneAIChoice::ReleaseAbility:
			{
				Payload.CommandType = ETheOneUseAbilityCommandType::UseAbility;
				Payload.IntPayload = FinalChoice.AbilityIndex;
				Payload.ActorPayload = FinalChoice.TargetCharacter;
			}
			break;
	}

	EventSystem->UseAbilityCommand.Broadcast(Payload);
}

void ATheOneBattle::OnCharacterEndTurn(ATheOneCharacterBase* InCharacter)
{
	auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
	check(CurrentCharacter == InCharacter)
	CompleteWaitSignal();
}

void ATheOneBattle::OnCommandBehaviorEnd(bool bBoolPayload)
{
	if (BattleContext.Stage == ETheOneBattleStage::GameOver)
	{
		UE_LOG(LogTheOne, Log, TEXT("OnCommandBehaviorEnd, 此时游戏结束， 未执行后续逻辑"));
		return;
	}
	
	auto CurrentCharacter = BattleContext.ActionQueue[BattleContext.CurrentTurn];
	if (IInHexActorInterface::Execute_GetCamp(CurrentCharacter) == ETheOneCamp::Enemy)
	{
		// NextThink
		// 下一帧执行逻辑判断要不要
		check(!BattleContext.WantToNextAIThink)
		BattleContext.WantToNextAIThink = true;
	}
}

void ATheOneBattle::OnCharacterDead(ATheOneCharacterBase* InCharacter)
{
	// 检测战斗是否结束
	auto SelfTeam = GetCharactersByCamp(IInHexActorInterface::Execute_GetCamp(InCharacter));
	// 检测是否全部死亡
	bool HasAnyAlive = false;
	for (auto Character : SelfTeam)
	{
		if (!ITheOneBattleInterface::Execute_IsDead(Character))
		{
			HasAnyAlive = true;
			break;
		}
	}

	if (!HasAnyAlive)
	{
		// 战斗结束
		UE_LOG(LogTheOne, Log, TEXT("战斗结束"));
		BattleContext.WantToGameOver = true;
	}
	else
	{
		// 如果死亡角色是当前行动角色
		if (BattleContext.ActionQueue[BattleContext.CurrentTurn] == InCharacter)
		{
			// Todo: 在行为树中执行“反应”的节点后，增加一个检查是否死亡的节点， 如果死亡，中断行为树，防止后续BehaviorEnd的抛出
			UE_LOG(LogTheOne, Error, TEXT("当前行动角色死亡, Todo: "));
		}
	}
}

TArray<ATheOneCharacterBase*> ATheOneBattle::GetCharactersByCamp(ETheOneCamp InCamp) const
{
	TArray<ATheOneCharacterBase*> Characters;
	for (const auto& Pair:InBattleCharacters)
	{
		if (IInHexActorInterface::Execute_GetCamp(Pair.Value.Get()) == InCamp)
		{
			Characters.Add(Pair.Value.Get());
		}
	}

	return Characters;
}

const TArray<FTheOneAIChoice>& ATheOneBattle::GetChessMemory(ATheOneCharacterBase* InSelf)
{
	return BattleContext.ChessContextMap[InSelf->GetFlag()].Memory;
}

const FRandomStream& ATheOneBattle::GetRandomStream() const
{
	return BattleContext.RandomStream;
}
