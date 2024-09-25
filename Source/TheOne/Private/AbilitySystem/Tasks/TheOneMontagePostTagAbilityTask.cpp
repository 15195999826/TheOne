// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TheOneMontagePostTagAbilityTask.h"

UTheOneMontagePostTagAbilityTask* UTheOneMontagePostTagAbilityTask::CreateMontagePost(UGameplayAbility* OwningAbility,
	FGameplayTag EventTag, bool OnlyTriggerOnce)
{
	UTheOneMontagePostTagAbilityTask* RetTask = NewAbilityTask<UTheOneMontagePostTagAbilityTask>(OwningAbility);
	RetTask->Tag = EventTag;
	RetTask->OnlyTriggerOnce = OnlyTriggerOnce;
	return RetTask;
}

void UTheOneMontagePostTagAbilityTask::Activate()
{
	Super::Activate();

	auto TheOneASC = Cast<UTheOneAbilitySystemComponent>(AbilitySystemComponent);
	if (TheOneASC == nullptr)
	{
		EndTask();
		TaskFailed.Broadcast();
		return;
	}

	MyHandle = TheOneASC->MontageEventCallbacks.FindOrAdd(Tag).AddUObject(this,
		&UTheOneMontagePostTagAbilityTask::MontagePostCallback);
}

void UTheOneMontagePostTagAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	auto TheOneASC = Cast<UTheOneAbilitySystemComponent>(AbilitySystemComponent);
	if (TheOneASC && MyHandle.IsValid())
	{
		TheOneASC->MontageEventCallbacks.FindOrAdd(Tag).Remove(MyHandle);
	}
		
	Super::OnDestroy(bInOwnerFinished);
}


void UTheOneMontagePostTagAbilityTask::MontagePostCallback(FTheOneMontageEventData* PanguMontageEventData)
{
	EventReceived.Broadcast(*PanguMontageEventData);
		
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}
