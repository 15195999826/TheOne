// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/AbilityTask_BattleEventPak.h"

#include "AbilitySystemComponent.h"
#include "TheOneGameplayTags.h"

UAbilityTask_BattleEventPak* UAbilityTask_BattleEventPak::TheOneBattleEventPak(UGameplayAbility* OwningAbility)
{
	UAbilityTask_BattleEventPak* MyObj = NewAbilityTask<UAbilityTask_BattleEventPak>(OwningAbility);
	return MyObj;
}

void UAbilityTask_BattleEventPak::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		// Lambda function to handle the gameplay event
		auto Handle0 = ASC->GenericGameplayEventCallbacks.FindOrAdd(
			TheOneGameplayTags::Battle_Event_Attack_Hit).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Attack_Hit, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Attack_Hit, Handle0);
		auto Handle1 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_BeforeHit).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_BeforeHit, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_BeforeHit, Handle1);
		auto Handle2 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_Hit).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_Hit, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_Hit, Handle2);
		auto Handle3 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_Miss).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_Miss, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_Miss, Handle3);
		auto Handle4 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_BeforeBeHit).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_BeforeBeHit, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_BeforeBeHit, Handle4);
		auto Handle5 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_BeHit).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_BeHit, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_BeHit, Handle5);
		auto Handle6 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_Dodge).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_Dodge, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_Dodge, Handle6);
		auto Handle7 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_Projectile_End).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_Projectile_End, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_Projectile_End, Handle7);
		auto Handle8 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_EnterZOC).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_EnterZOC, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_EnterZOC, Handle8);
		// AfterMove
		auto Handle9 = ASC->GenericGameplayEventCallbacks.FindOrAdd(TheOneGameplayTags::Battle_Event_LeaveZOC).AddLambda([this](const FGameplayEventData* Payload)
		{
			GameplayEventCallback(TheOneGameplayTags::Battle_Event_LeaveZOC, Payload);
		});
		BattleEventHandleMap.Add(TheOneGameplayTags::Battle_Event_LeaveZOC, Handle9);
	}

	
	Super::Activate();
}

void UAbilityTask_BattleEventPak::OnDestroy(bool bInOwnerFinished)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		for (auto& Pair : BattleEventHandleMap)
		{
			if (Pair.Value.IsValid())
			{
				ASC->GenericGameplayEventCallbacks.FindOrAdd(Pair.Key).Remove(Pair.Value);
			}
		}
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_BattleEventPak::GameplayEventCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload) const
{ 
	ensureMsgf(Payload, TEXT("GameplayEventCallback expected non-null Payload"));
	FGameplayEventData TempPayload = Payload ? *Payload : FGameplayEventData{};
	TempPayload.EventTag = MatchingTag;
	EventReceived.Broadcast(MoveTemp(TempPayload));
}
