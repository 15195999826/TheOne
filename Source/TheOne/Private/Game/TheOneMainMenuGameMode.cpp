// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOneMainMenuGameMode.h"

#include "TheOneLogChannels.h"
#include "Types/TheOneCharacterConfigStruct.h"
#include "Development/TheOneDataTableSettings.h"

void ATheOneMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	// 打开UI
	auto DebugDT = GetDefault<UTheOneDataTableSettings>()->CharacterTemplateTable;
	bool IsValid = DebugDT->IsValidLowLevel();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("DebugDT is valid: ") + (IsValid ? FString("true") : FString("false")));

	// 屏幕打印全部的MinionData
	TArray<FTheOneCharacterConfig*> MinionDataArray;
	DebugDT->GetAllRows<FTheOneCharacterConfig>("", MinionDataArray);
	for (auto& MinionData : MinionDataArray)
	{
		auto Name = MinionData->Custom.MinionName;
		UE_LOG(LogTheOne, Log, TEXT("MinionName: %s"), *Name.ToString());
	}
	
	// 设置声音
	SetUpSound();
}
