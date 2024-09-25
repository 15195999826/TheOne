// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneDataDrivePassiveGA.h"

int32 UTheOneDataDrivePassiveGA::RegisterBattleEvent(FGameplayTag InBattleEvent, FName InAbilityConfigName)
{
	auto& MapArray = BattleEvent2AbilityConfigMap.FindOrAdd(InBattleEvent);
	
	FTheOneDataDriveBattleEvent NewEvent;
	NewEvent.EventID = SelfEventID++;
	NewEvent.AbilityConfigName = InAbilityConfigName;
	MapArray.Add(NewEvent);

	return NewEvent.EventID;
}

void UTheOneDataDrivePassiveGA::RemoveBattleEvent(int32 InEventID)
{
	for (auto& MapArray : BattleEvent2AbilityConfigMap)
	{
		for (int32 i = 0; i < MapArray.Value.Num(); ++i)
		{
			if (MapArray.Value[i].EventID == InEventID)
			{
				MapArray.Value.RemoveAt(i);
				return;
			}
		}
	}
}

void UTheOneDataDrivePassiveGA::SetupDataDrivePassiveAbility()
{
	SetupAbility(ETheOneAbilityType::Passive, 1, nullptr);
}

void UTheOneDataDrivePassiveGA::ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload)
{
	auto InTag = InBattleEventPayload.EventTag;
	auto AbilityDT = GetDefault<UTheOneDataTableSettings>()->AbilityTable.LoadSynchronous();
	// if (auto MapArray = BattleEvent2AbilityConfigMap.Find(InTag))
	// {
	// 	for (auto& Event : *MapArray)
	// 	{
	// 		auto AbilityConfig = AbilityDT->FindRow<FTheOneAbilityConfig>(Event.AbilityConfigName, "ReceiveBattleEvent");
	// 		if (!AbilityConfig)
	// 		{
	// 			continue;
	// 		}
	// 		
	// 		// Todo: 检测CD和次数限制
	// 		for (auto& ActionPoint : AbilityConfig->ActionPoints)
	// 		{
	// 			if (ActionPoint.ActionPoint == InTag)
	// 			{
	// 				// 执行技能
	// 				for (auto& Action : ActionPoint.Actions)
	// 				{
	// 					// Todo: 关于FormAbility， 被动技能该如何处理的方案
	// 					Action.DoAction(GetAvatarActorFromActorInfo(), Level, this, AbilityConfig->DrawDebug);
	// 				}
	// 			}
	// 		}
	// 	} 
	// }
}
