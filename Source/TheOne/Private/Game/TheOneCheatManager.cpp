// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneCheatManager.h"

#include "AI/AutoChess//TheOneAutoChessAIController.h"
#include "Character/TheOneCharacterBase.h"
#include "Development/TheOneDeveloperSettings.h"
#include "Game/TheOneGameModeBase.h"
#include "Game/PlayerControllers//TheOneHexMapPlayerController.h"
#include "HexGrid/HexPathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogTheOneCheat);

void UTheOneCheatManager::InitCheatManager()
{
	Super::InitCheatManager();
	auto DeveloperSettings = GetDefault<UTheOneDeveloperSettings>();
	bEnableCheat = DeveloperSettings->EnableCheat;

	auto BattleGameWorld = Cast<ATheOneGameModeBase>(GetWorld()->GetAuthGameMode());
	if (BattleGameWorld)
	{
		BattleGameWorld->OnLevelPrepared.AddUObject(this, &UTheOneCheatManager::OnLevelPrepared);
	}
}

void UTheOneCheatManager::CheatAddOneAI()
{
	CheatExecute(ECheatFunction::CheatAddOneAI);
}

void UTheOneCheatManager::TestCheat()
{
	CheatExecute(ECheatFunction::TestCheat);
}

void UTheOneCheatManager::TestCheatParam(const FString& Msg)
{
	CheatExecute(ECheatFunction::TestCheatParam, Msg);
}

void UTheOneCheatManager::ToggleDebugHexPath()
{
	if (!bEnableCheat)
	{
		UE_LOG(LogTheOneCheat, Warning, TEXT("Cheat is disabled"));
		return;
	}

	auto Controller = Cast<ATheOneHexMapPlayerController>(GetOuterAPlayerController());
	if (Controller)
	{
		auto SelectedCharacter = Controller->GetSelectedCharacter();
		if (SelectedCharacter)
		{
			auto Ctrl = SelectedCharacter->GetController();
			if (Ctrl)
			{
				auto AICtrl = Cast<ATheOneAutoChessAIController>(Ctrl);
				if (AICtrl)
				{
					auto PathFollowingComponent = AICtrl->GetPathFollowingComponent();
					if (auto HexPath = Cast<UHexPathFollowingComponent>(PathFollowingComponent))
					{
						HexPath->DrawDebug = !HexPath->DrawDebug;
					}
				}
			}
		}
	}
}

void UTheOneCheatManager::CheatExecute(ECheatFunction Function, const FString& Msg)
{
	if (!bEnableCheat)
	{
		UE_LOG(LogTheOneCheat, Warning, TEXT("Cheat is disabled"));
		return;
	}

	switch (Function) {
	case ECheatFunction::None:
		break;
	case ECheatFunction::CheatAddOneAI:
		UE_LOG(LogTheOneCheat, Log, TEXT("CheatAddOneAI"));
		break;
	case ECheatFunction::TestCheat:
		UE_LOG(LogTheOneCheat, Log, TEXT("TestCheat"));
		
		break;
	case ECheatFunction::TestCheatParam:
		UE_LOG(LogTheOneCheat, Log, TEXT("TestCheatParam: %s"), *Msg);
		break;
	}
}

void UTheOneCheatManager::OnLevelPrepared()
{
	APlayerController* PC = GetOuterAPlayerController();
	auto DeveloperSettings = GetDefault<UTheOneDeveloperSettings>();
	if (DeveloperSettings->CheatIndex >= 0 && DeveloperSettings->CheatsToRun.Num() > DeveloperSettings->CheatIndex)
	{
		auto Cheat = DeveloperSettings->CheatsToRun[DeveloperSettings->CheatIndex];
		for (const FTheOneCheatRow& CheatRow : Cheat.OnLevelLoadedCheat)
		{
			CheatExecute(CheatRow.CheatFunction, CheatRow.CheatParam);
		}
	}
}
