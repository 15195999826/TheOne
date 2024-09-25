// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoChessLevelEditSystem.h"

#include "DataTableEditorUtils.h"
#include "FileHelpers.h"
#include "Development/TheOneDataTableSettings.h"
#include "Game/TheOneEventSystem.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/PlayerControllers/TheOneHexMapPlayerController.h"

class UTheOneDataTableSettings;
class UTheOneEventSystem;

void UAutoChessLevelEditSystem::OnWorldComponentsUpdated(UWorld& World)
{
	Super::OnWorldComponentsUpdated(World);

	auto GameMode = World.GetAuthGameMode();

	if (GameMode == nullptr)
	{
		return;
	}

	auto TheOneGameMode = Cast<ATheOneGameModeBase>(GameMode);
	if (TheOneGameMode)
	{
		TheOneGameMode->OnLevelPrepared.AddUObject(this, &UAutoChessLevelEditSystem::OnLevelPrepared);
	}
}

// 输入const FTheOneAutoChessLevel& RowData, 在Destruct时会报错
void UAutoChessLevelEditSystem::OnTheOneSaveLevel(const FName& RowName, const FTheOneAutoChessLevelConfig LevelData)
{
	auto DTSettings = GetDefault<UTheOneDataTableSettings>();
	auto DT = DTSettings->LevelTable.LoadSynchronous();
	auto Row = DT->FindRow<FTheOneAutoChessLevelConfig>(RowName, TEXT(""));
	if (Row == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTheOneSaveLevel,Row:%s is null"), *RowName.ToString());
		return;
	}
	
	Row->AIInfos.Empty();

	UE_LOG(LogTemp, Warning, TEXT("OnTheOneSaveLevel After Empty,RowNum:%d, InRowNum:%d"), Row->AIInfos.Num(), LevelData.AIInfos.Num());
	
	for (const auto AIInfo : LevelData.AIInfos)
	{
		Row->AIInfos.Add(AIInfo);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnTheOneSaveLevel After Add,RowNum:%d, InRowNum:%d"), Row->AIInfos.Num(), LevelData.AIInfos.Num());
	// 打印Row的AIInfos
	for (const auto AIInfo : Row->AIInfos)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIInfo, Row:%d, Column:%d, MinionTemplateRow:%s"), AIInfo.Row, AIInfo.Column, *AIInfo.MinionRow.RowName.ToString());
	}
	
	UEditorLoadingAndSavingUtils::SavePackages({DT->GetPackage()}, false);
	FDataTableEditorUtils::BroadcastPostChange(DT, FDataTableEditorUtils::EDataTableChangeInfo::RowData);

}

void UAutoChessLevelEditSystem::OnLevelPrepared()
{
	UE_LOG(LogTemp, Warning, TEXT("UAutoChessLevelEditSystem::OnLevelPrepared"));
	auto PC = GetWorld()->GetFirstPlayerController();
	if (PC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PC IS NULL"));
		return;
	}

	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();

	EventSystem->OnTheOneSaveLevel.AddDynamic(this, &UAutoChessLevelEditSystem::OnTheOneSaveLevel);

	auto TheOneHexPC = Cast<ATheOneHexMapPlayerController>(PC);
	if (TheOneHexPC)
	{
		TheOneHexPC->PlayInEditor();
	}
}
