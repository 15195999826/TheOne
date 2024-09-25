// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GameModeComponents/TheOneAutoChessBattleComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "AutoChessLevelEditSystem.generated.h"

/**
 * 
 */
UCLASS()
class THEONEEDITOR_API UAutoChessLevelEditSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldComponentsUpdated(UWorld& World) override;
protected:
	UFUNCTION()
	void OnTheOneSaveLevel(const FName& RowName, const FTheOneAutoChessLevelConfig LevelData);
	void OnLevelPrepared();
};
