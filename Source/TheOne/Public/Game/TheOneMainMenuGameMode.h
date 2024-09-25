// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TheOneMainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API ATheOneMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUpSound();
};
