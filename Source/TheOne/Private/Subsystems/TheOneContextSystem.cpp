// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TheOneContextSystem.h"

void UTheOneContextSystem::RegisterGameplayEventWaiting(AActor* Actor)
{
	GameplayEventWaitingActors.Add(Actor);
}

void UTheOneContextSystem::UnRegisterGameplayEventWaiting(AActor* Actor)
{
	GameplayEventWaitingActors.Remove(Actor);
	if (GameplayEventWaitingActors.Num() == 0)
	{
		OnGameplayEventTriggerOver.Broadcast();
		OnGameplayEventTriggerOver.Clear();
	}
}

void UTheOneContextSystem::DebugGameplayEventWaitingActors()
{
	for (auto& Actor : GameplayEventWaitingActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Waiting Actor: %s"), *Actor->GetName());
	}
}