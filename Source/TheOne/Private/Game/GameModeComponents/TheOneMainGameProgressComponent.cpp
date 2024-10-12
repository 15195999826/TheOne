// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModeComponents/TheOneMainGameProgressComponent.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "Game/TheOneBattle.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/Battle/TheOneBattleEvaluate.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"
#include "Types/TheOneImportantUI.h"

// Sets default values for this component's properties
UTheOneMainGameProgressComponent::UTheOneMainGameProgressComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTheOneMainGameProgressComponent::BeginPlay()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
 	ContextSystem->Battle = GetWorld()->SpawnActor<ATheOneBattle>(BattleLogicClass, FTransform::Identity);
	ContextSystem->BattleEvaluate = GetWorld()->SpawnActor<ATheOneBattleEvaluate>(BattleEvaluateClass, FTransform::Identity);
	WeakBattle = ContextSystem->Battle;
	Super::BeginPlay();
	auto GameMode = Cast<ATheOneGameModeBase>(GetOwner());
	if (GameMode)
	{
		GameMode->OnLevelPrepared.AddUObject(this, &UTheOneMainGameProgressComponent::OnLevelPrepared);
	}
}

void UTheOneMainGameProgressComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	WeakBattle->BattleTick();
}

// Todo: 战斗结束删除敌人队伍
void UTheOneMainGameProgressComponent::EnterBattle()
{
	// 隐藏MainWindow
	UTheOneBlueprintFunctionLibrary::CloseImportantUI(this, ETheOneImportantUI::MainWindow);
	// 显示BattleWindow
	UTheOneBlueprintFunctionLibrary::ShowImportantUI(this, ETheOneImportantUI::BattleWindow);
	WeakBattle->OnEnterBattle();
}

void UTheOneMainGameProgressComponent::OnLevelPrepared()
{
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	HexGrid->Empty();
}
