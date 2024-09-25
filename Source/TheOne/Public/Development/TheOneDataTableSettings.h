// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TheOneDataTableSettings.generated.h"

class ATheOneAIController;
class ATheOneCharacterBase;
class UTheOneGameplayAbility;
/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class UTheOneDataTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<ATheOneCharacterBase> DefaultCharacterClass;

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<ATheOneAIController> DefaultAIControllerClass;
	
	UPROPERTY(config, EditAnywhere)
	TSubclassOf<UTheOneGameplayAbility> DefaultTableAbility;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> LevelTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> CharacterTemplateTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> AbilityTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> ProjectileTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> WeaponTable;
	
	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> PropTable;
	
	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> BuffTable;
};
