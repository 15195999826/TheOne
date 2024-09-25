// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TheOneTestAbilityTask.generated.h"

/**
 * AbilityTask 会随着 Ability 的End结束而结束， 不需要管理它的生命周期
 */
UCLASS()
class THEONE_API UTheOneTestAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

	virtual void OnDestroy(bool AbilityEnded) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTheOneTestAbilityTask* CreateTheOneTestAbilityTask(UGameplayAbility* OwningAbility);
	
protected:
	virtual void Activate() override;
	
};
