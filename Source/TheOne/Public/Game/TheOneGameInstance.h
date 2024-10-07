// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RegisterInputActionTask.h"
#include "Engine/GameInstance.h"
#include "TheOneGameInstance.generated.h"

class UTheOneUIRoot;

USTRUCT(BlueprintType)
struct FTheOneTravelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle LevelRow;
};

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UTheOneUIRoot> UIRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTheOneTravelInfo TravelInfo;

	UFUNCTION(BlueprintImplementableEvent)
	void ProtectInitUIRoot();
};
