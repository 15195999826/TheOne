// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"
#include "TheOneTestAbilityAsync.generated.h"

/**
 * Async Task 不会随着 Ability 的结束而结束，需要手动结束， 它不应该用于GameplayAbility中‘
 * 像UAbilityTask_WaitGameplayEvent这样的任务, 结束技能时不需要手动End
 */
UCLASS()
class THEONE_API UTheOneTestAbilityAsync : public UAbilityAsync
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TheOne|Ability|Tasks", meta = (DisplayName="Wait TheOneTest", DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UTheOneTestAbilityAsync* CreateTheOneTest(AActor* TargetActor, FGameplayTag EventTag, bool OnlyTriggerOnce = false);

protected:
	virtual void Activate() override;
	virtual void EndAction() override;
	virtual void Cancel() override;
};

inline void UTheOneTestAbilityAsync::Cancel()
{
	Super::Cancel();
	UE_LOG(LogTemp, Log, TEXT("UTheOneTestAbilityAsync::Cancel"));
}
