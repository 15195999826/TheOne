// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_BattleEventPak.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTheOneWaitGameplayEventDelegate, FGameplayEventData, Payload);

/**
 * 
 */
UCLASS()
class THEONE_API UAbilityTask_BattleEventPak : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_BattleEventPak* TheOneBattleEventPak(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FTheOneWaitGameplayEventDelegate EventReceived;
	
protected:
	TMap<FGameplayTag, FDelegateHandle> BattleEventHandleMap;
	
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void GameplayEventCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload) const;
};
