// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModeComponents/TheOneAutoChessBattleComponent.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/TheOneAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Development/TheOneDeveloperSettings.h"
#include "Game/TheOneGameInstance.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"
#include "HexGrid/HexGrid.h"
#include "Item/TheOneItemSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTheOneAutoChessBattleComponent::UTheOneAutoChessBattleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTheOneAutoChessBattleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	auto GameMode = GetGameMode();
	GameMode->OnLevelPrepared.AddUObject(this, &UTheOneAutoChessBattleComponent::OnLevelPrepared);
}

void UTheOneAutoChessBattleComponent::BattleTick(float DeltaTime)
{
	
}

void UTheOneAutoChessBattleComponent::OnAICharacterDead()
{
	// 检查游戏是否结束
	bool AllEnemyDead = true;
	for (auto& EnemyCtrl : EnemyCtrls)
	{
		if (EnemyCtrl.IsValid())
		{
			if (!EnemyCtrl->IsDead())
			{
				AllEnemyDead = false;
				break;
			}
		}
	}
	
	if (AllEnemyDead)
	{
		OnBattleOver(true);
		return;
	}

	bool AllPlayerDead = true;
	for (auto& PlayerCtrl : PlayerCtrls)
	{
		if (PlayerCtrl.IsValid())
		{
			if (!PlayerCtrl->IsDead())
			{
				AllPlayerDead = false;
				break;
			}
		}
	}

	if (AllPlayerDead)
	{
		OnBattleOver(false);
	}
}

void UTheOneAutoChessBattleComponent::OnBattleOver(bool InPlayerWin)
{
	UE_LOG(LogTheOne, Log, TEXT("[AutoChess]AllEnemyDead, %s"), InPlayerWin ? TEXT("玩家胜利") : TEXT("玩家失败"));
	// 暂停全部AI
	for (auto& PlayerCtrl : PlayerCtrls)
	{
		if (PlayerCtrl.IsValid())
		{
			PlayerCtrl->BehaviorTreeComponent->StopTree();
		}
	}

	for (auto& EnemyCtrl : EnemyCtrls)
	{
		if (EnemyCtrl.IsValid())
		{
			EnemyCtrl->BehaviorTreeComponent->StopTree();
		}
	}
}


void UTheOneAutoChessBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTicking)
	{
		return;
	}

	BattleTick(DeltaTime);
}

const TArray<TWeakObjectPtr<ATheOneAutoChessAIController>>& UTheOneAutoChessBattleComponent::GetEnemies(
	ATheOneCharacterBase* InTheOneCharacter)
{
	auto Camp = InTheOneCharacter->GetCamp();
	switch (Camp) {
		case ETheOneCamp::Player:
			return EnemyCtrls;
		case ETheOneCamp::Enemy:
			return PlayerCtrls;
		default:
			return EmptyCtrls;
	}
}

void UTheOneAutoChessBattleComponent::OnLevelPrepared()
{
	UE_LOG(LogTheOne, Log, TEXT("UTheOneAutoChessBattleComponent::OnLevelPrepared"));
	// Todo: 
	// EnemyCtrls.Empty();
	// PlayerCtrls.Empty();
	//
	// const FTheOneTravelInfo& TravelInfo = CastChecked<UTheOneGameInstance>(GetWorld()->GetGameInstance())->TravelInfo;
	// UE_LOG(LogTheOne, Log, TEXT("TravelInfo.LevelRow: %s"), *TravelInfo.LevelRow.RowName.ToString());
	// // Todo: 读取LevelSetting中的数据，生成AI
	// auto LevelData = TravelInfo.LevelRow.GetRow<FTheOneAutoChessLevelConfig>(TEXT(""));
	// // Spawn AI
	// if (LevelData == nullptr)
	// {
	// 	// 临时Debug
	// 	UE_LOG(LogTheOne, Error, TEXT("UTheOneAutoChessBattleComponent::OnLevelPrepared: LevelData is nullptr, EnterDebugLevel"));
	// 	LevelData = GetDefault<UTheOneDeveloperSettings>()->DebugAutoChessLevel.GetRow<FTheOneAutoChessLevelConfig>(TEXT(""));
	// }
	//
	// check(LevelData);
	//
	// AHexGrid* HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(this);
	// check(HexGrid);
	//
	// auto GameMode = GetGameMode();
	//
	// for (const FTheOneLevelEnemy& LevelEnemy : LevelData->AIInfos)
	// {
	// 	// Todo: Position, Z轴高度是不是要放到角色配置中
	// 	auto Position = HexGrid->HexXYToWorld(LevelEnemy.Row, LevelEnemy.Column) + FVector(0.f, 0.f, 100.f);
	// 	FTransform SpawnTrans = FTransform(FRotator(0.f, 180.f, 0.f), Position);
	// 	FTheOneAIPawnSpawnInfo Info;
	// 	Info.CharacterTemplateRowName = LevelEnemy.MinionRow.RowName;
	// 	Info.Camp = ETheOneCamp::Enemy;
	// 	auto AICtrl = CastChecked<ATheOneAutoChessAIController>(GameMode->SpawnOneAIAtTransform(DefaultAIControllerClass, SpawnTrans, Info));
	// 	EnemyCtrls.Add(AICtrl);
	//
	// 	// Todo: 临时设置运行时数据
	// 	auto TheOneCharacter = Cast<ATheOneCharacterBase>(AICtrl->GetPawn());
	// 	auto Attr = TheOneCharacter->GetAttributeSet();
	// 	Attr->SetHealth(Attr->GetMaxHealth());
	// 	Attr->SetMana(0);
	// 	TheOneCharacter->OnceOnDead.AddUObject(this, &UTheOneAutoChessBattleComponent::OnAICharacterDead);
	// }
	//
	// // 创建我方角色， Todo: 临时创建TestPlayerAIInfos
	// TArray<FTheOneLevelEnemy> TestPlayerAIInfos = GetDefault<UTheOneDeveloperSettings>()->TestPlayerAIInfos;
	// for (const FTheOneLevelEnemy& PlayerMinion : TestPlayerAIInfos)
	// {
	// 	auto Position = HexGrid->HexXYToWorld(PlayerMinion.Row, PlayerMinion.Column) + FVector(0.f, 0.f, 100.f);
	// 	FTransform SpawnTrans = FTransform(FRotator(0.f, 180.f, 0.f), Position);
	// 	FTheOneAIPawnSpawnInfo Info;
	// 	Info.CharacterTemplateRowName = PlayerMinion.MinionRow.RowName;
	// 	Info.Camp = ETheOneCamp::Player;
	// 	auto AICtrl = CastChecked<ATheOneAutoChessAIController>(GameMode->SpawnOneAIAtTransform(DefaultAIControllerClass, SpawnTrans, Info));
	// 	PlayerCtrls.Add(AICtrl);
	//
	// 	// Todo: 临时设置运行时数据
	// 	auto TheOneCharacter = Cast<ATheOneCharacterBase>(AICtrl->GetPawn());
	// 	auto Attr = TheOneCharacter->GetAttributeSet();
	// 	Attr->SetHealth(Attr->GetMaxHealth());
	// 	Attr->SetMana(0);
	// 	TheOneCharacter->OnceOnDead.AddUObject(this, &UTheOneAutoChessBattleComponent::OnAICharacterDead);
	// }
	//
	// // 开始自走棋战斗
	// bTicking = true;
}

ATheOneGameModeBase* UTheOneAutoChessBattleComponent::GetGameMode() const
{
	return CastChecked<ATheOneGameModeBase>(GetOwner());
}
