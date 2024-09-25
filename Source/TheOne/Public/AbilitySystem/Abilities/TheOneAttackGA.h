// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "Types/TheOneAttackAbility.h"
#include "TheOneAttackGA.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTheOneAttackPointReachedSignature, bool, bIsLastAttackPoint);
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAttackGA : public UTheOneGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	float NormalizeRate;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> AttackPointPercents;

	UPROPERTY(BlueprintReadOnly)
	ETheOneAttackCapability AttackCapability;
	
	/**
	 * 返回值表示是否是最后一个攻击点
	 */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FTheOneAttackPointReachedSignature OnAttackPointReached;
	
	UPROPERTY()
	FTheOneAbilityActionContainer ActionContainer;
	
public:
	void SetupAttackAbility(const FTheOneAttackAbility& InAttackAbility);


	// AttackPoint Execution
protected:
	UFUNCTION(BlueprintCallable)
	void NativeOnAttackPoint(bool bIsLastAttackPoint);
};
