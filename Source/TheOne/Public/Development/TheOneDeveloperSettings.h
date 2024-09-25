// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "TheOneDeveloperSettings.generated.h"

class UGameplayEffect;
struct FTheOneLevelEnemy;

UENUM(BlueprintType)
enum class ECheatFunction : uint8
{
	None,
	CheatAddOneAI UMETA(DisplayName = "添加一个AI"),
	TestCheat UMETA(DisplayName = "测试Cheat"),
	TestCheatParam UMETA(DisplayName = "测试CheatParam"),
};

USTRUCT()
struct FTheOneCheatRow
{
	GENERATED_BODY()

	FTheOneCheatRow()
		: CheatFunction(ECheatFunction::None)
		, CheatParam(TEXT(""))
	{
	}

	UPROPERTY(EditAnywhere)
	ECheatFunction CheatFunction;
	
	UPROPERTY(EditAnywhere)
	FString CheatParam;
};

USTRUCT()
struct FTheOneCheatToRun
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString CheatName;
	
	UPROPERTY(EditAnywhere)
	TArray<FTheOneCheatRow> OnLevelLoadedCheat;
};


/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class UTheOneDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:
	virtual FName GetCategoryName() const override;

public:
	UPROPERTY(config, EditAnywhere, Category=Cheat)
	bool EnableCheat = true;
	
	UPROPERTY(config, EditAnywhere, Category=Cheat)
	int CheatIndex = -1;

	UPROPERTY(config, EditAnywhere, Category=Cheat)
	TArray<FTheOneCheatToRun> CheatsToRun;

	UPROPERTY(config, EditAnywhere, Category = Cheat)
	FDataTableRowHandle DebugAutoChessLevel;
	
	UPROPERTY(config, EditAnywhere, Category = Cheat)
	TArray<FTheOneLevelEnemy> TestPlayerAIInfos;
	
#if WITH_EDITORONLY_DATA
	/** A list of common maps that will be accessible via the editor detoolbar */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Maps, meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> CommonEditorMaps;
#endif
	
};
