// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TheOneTestAbilityTask.h"

void UTheOneTestAbilityTask::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
	UE_LOG(LogTemp, Log, TEXT("USeddTestAbilityTask::OnDestroy, AbilityEnded: %d"), AbilityEnded);
}

UTheOneTestAbilityTask* UTheOneTestAbilityTask::CreateTheOneTestAbilityTask(UGameplayAbility* OwningAbility)
{
	UTheOneTestAbilityTask* MyObj = NewAbilityTask<UTheOneTestAbilityTask>(OwningAbility);
	return MyObj;
}

void UTheOneTestAbilityTask::Activate()
{
	Super::Activate();
	UE_LOG(LogTemp, Log, TEXT("USeddTestAbilityTask::Activate"));
}
