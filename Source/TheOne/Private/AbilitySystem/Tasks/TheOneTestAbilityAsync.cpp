// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/TheOneTestAbilityAsync.h"

UTheOneTestAbilityAsync* UTheOneTestAbilityAsync::CreateTheOneTest(AActor* TargetActor, FGameplayTag EventTag,
                                                                   bool OnlyTriggerOnce)
{
	UTheOneTestAbilityAsync* MyObj = NewObject<UTheOneTestAbilityAsync>();
	return MyObj;
}

void UTheOneTestAbilityAsync::Activate()
{
	Super::Activate();
	UE_LOG(LogTemp, Log, TEXT("UTheOneTestAbilityAsync::Activate"));
}

void UTheOneTestAbilityAsync::EndAction()
{
	Super::EndAction();
	UE_LOG(LogTemp, Log, TEXT("UTheOneTestAbilityAsync::EndAction"));
}
