// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "Interface/TheOneAbilityEvaInterface.h"
#include "TheOneGeneralGA.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneGeneralGA : public UTheOneGameplayAbility, public ITheOneAbilityEvaInterface
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

	const FDataTableRowHandle& GetAbilityRowHandle() const { return AbilityRow; }
	const FTheOneAbilityConfig* GetConfig();

protected:
	UFUNCTION(BlueprintCallable)
	void DoAction(FGameplayTag InActionPoint);

	// For Evaluate
public:
	virtual void PredictDamage_Implementation(AActor* Target, float& OutDamage, bool& OutMaybeDead) override;
};
