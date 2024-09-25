// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneAttackGA.h"
#include "Engine/DeveloperSettings.h"
#include "TheOneGeneralSettings.generated.h"

class UTheOneDataDrivePassiveGA;
/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class UTheOneGeneralSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;

	UPROPERTY(config, EditAnywhere, Category = Gameplay)
	TSubclassOf<UGameplayEffect> InitVitalAttributeSetEffect;

	UPROPERTY(config, EditAnywhere, Category = Gameplay)
	TSubclassOf<UGameplayEffect> InitWeaponVitalAttributeSetEffect;

	UPROPERTY(config, EditAnywhere, Category = Gameplay)
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	UPROPERTY(config, EditAnywhere, Category=Character)
	TSubclassOf<UTheOneAttackGA> DefaultAttackGA;

	UPROPERTY(config, EditAnywhere, Category=Character)
	TSubclassOf<UTheOneDataDrivePassiveGA> DefaultDTDrivePassiveGA;
};
