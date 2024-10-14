// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TheOneAbilityEvaInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTheOneAbilityEvaInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEONE_API ITheOneAbilityEvaInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 预测造成的伤害
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TheOne|AbilityEva")
	void PredictDamage(AActor* Target, float& OutDamage, bool& OutMaybeDead);
};
