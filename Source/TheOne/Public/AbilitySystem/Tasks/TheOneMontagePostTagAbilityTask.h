// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"
#include "TheOneMontagePostTagAbilityTask.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneMontagePostTagAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TheOne|Ability|Tasks", meta = (DisplayName="Wait MontagePost", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTheOneMontagePostTagAbilityTask* CreateMontagePost(UGameplayAbility* OwningAbility, FGameplayTag EventTag, bool OnlyTriggerOnce = false);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventReceivedDelegate, FTheOneMontageEventData, Payload);
	
	UPROPERTY(BlueprintAssignable)
	FEventReceivedDelegate EventReceived;

	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate TaskFailed;
	
protected:
	FGameplayTag Tag;
	bool OnlyTriggerOnce = false;
	FDelegateHandle MyHandle;

	void MontagePostCallback(FTheOneMontageEventData* PanguMontageEventData);
};
