// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TheOneAbilitySystemComponent.generated.h"

class UTheOneGameplayAbility;

USTRUCT(BlueprintType)
struct FTheOneMontageEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	TArray<AActor*> TargetActors;
};

UENUM(BlueprintType)
enum class ETheOneTryActiveResult : uint8
{
	Succeed,
	SucceedNoInstanced,
	Failed
};
/**
 *
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FTheOneMontagePostDelegate, FTheOneMontageEventData*)
UCLASS()
class THEONE_API UTheOneAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTheOneAbilitySystemComponent();


public:
	TMap<FGameplayTag, FTheOneMontagePostDelegate> MontageEventCallbacks;
	
public:
	void HandleMontagePostEvent(FGameplayTag EventTag, FTheOneMontageEventData EventData);

	// Out OutTheOneAbility
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs = "Success"))
	UTheOneGameplayAbility* TryTheOneActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, ETheOneTryActiveResult& Success);
	
protected:
	void OnBattleGameplayEffectAppliedToSelf(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	void OnBattleGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect);
};
