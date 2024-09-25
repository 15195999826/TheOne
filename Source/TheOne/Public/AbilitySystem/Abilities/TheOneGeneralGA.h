// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "TheOneGeneralGA.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneGeneralGA : public UTheOneGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	FDataTableRowHandle AbilityRow;

	UPROPERTY()
	TArray<FTheOneAbilityActionPoint> ActionPoints;

public:
	void SetUpGeneralAbility(const FDataTableRowHandle& InRow, const FTheOneAbilityConfig* InAbilityConfig, int InLevel);
	
	ETheOneAbilityReleaseTarget GetTargetType() const;
	
	ETheOneAbilityReleaseRule GetReleaseRule() const;

	const FDataTableRowHandle& GetAbilityRowHandle() const { return AbilityRow; }
protected:
	UFUNCTION(BlueprintCallable)
	void DoAction(FGameplayTag InActionPoint);
};
