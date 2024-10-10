// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneDataDrivePassiveGA.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "Subsystems/TheOneContextSystem.h"

int32 UTheOneDataDrivePassiveGA::RegisterBattleEvent(const FTheOneAbilityConfig* AbilityRow)
{
	check(AbilityRow->AbilitySource == ETheOneAbilitySource::DataTableDrive && AbilityRow->AbilityType == ETheOneAbilityType::Passive);
	auto EventID = SelfEventID++;
	if (AbilityRow->ActionPoints.Num() > 0)
	{
		check(AbilityRow->AbilityMontage.IsValid())
		TArray<FTheOneAbilityActionPoint> MontageActionPoints;
		for (auto& ActionPoint : AbilityRow->ActionPoints)
		{
			MontageActionPoints.Add(UTheOneBlueprintFunctionLibrary::CastConfigToAbilityActionPoint(ActionPoint));
		}
		check(EventID2MontageActionPointsMap.Contains(EventID) == false);
		EventID2MontageActionPointsMap.Add(EventID, MontageActionPoints);
		EventID2MontageMap.Add(EventID, AbilityRow->AbilityMontage.LoadSynchronous());
	}

	// Todo: 实现不带动画的被动技能的效果
	// if (AbilityRow->PassiveAbilityData)
	// {
	// 	
	// }

	// Todo: 未来可以增加除了GameplayTag之外的触发条用，用作接收事件后， 想要触发被动技能时的检测

	// 将EventID写如BattleEvents
	for (auto& Tag : AbilityRow->PassiveAbilityData.TriggerActionPoint)
	{
		auto& EventArray = BattleEvents.FindOrAdd(Tag);
		EventArray.Add(EventID);
	}

	return EventID;
}

void UTheOneDataDrivePassiveGA::RemoveBattleEvent(int32 InEventID)
{
	for (auto& Pair : BattleEvents)
	{
		if (Pair.Value.Contains(InEventID))
		{
			Pair.Value.Remove(InEventID);
			break;
		}
	}

	for (auto& Pair : EventID2MontageActionPointsMap)
	{
		if (Pair.Key == InEventID)
		{
			EventID2MontageActionPointsMap.Remove(InEventID);
			break;
		}
	}

	for (auto& Pair : EventID2ActionMap)
	{
		if (Pair.Key == InEventID)
		{
			EventID2ActionMap.Remove(InEventID);
			break;
		}
	}
	
	for (auto& Pair : EventID2MontageMap)
	{
		if (Pair.Key == InEventID)
		{
			EventID2MontageMap.Remove(InEventID);
			break;
		}
	}
}

void UTheOneDataDrivePassiveGA::SetupDataDrivePassiveAbility()
{
	SetupAbility(ETheOneAbilityType::Passive, 1, nullptr);
}

AActor* UTheOneDataDrivePassiveGA::GetCurrentEventTriggerActor() const
{
	auto Trigger =const_cast<UObject*>(CurrentEventPayload.OptionalObject.Get());
	return Cast<AActor>(Trigger);
}

TArray<int32> UTheOneDataDrivePassiveGA::ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload)
{
	auto InTag = InBattleEventPayload.EventTag;
	TArray<int32> WithMontageEventIDs;
	CurrentEventPayload = InBattleEventPayload;
	if (auto Pair = BattleEvents.Find(InTag))
	{
		for (auto& EventID : *Pair)
		{
			// Todo: TArray CanExecutedEventIDs;
			
			// 先直接把不需要蒙太奇的被动功能执行完毕
			if (EventID2ActionMap.Contains(EventID))
			{
				auto& ActionContainer = EventID2ActionMap[EventID];
				for (FTheOneAbilityAction& Action : ActionContainer.Actions)
				{
					Action.DoAction(GetAvatarActorFromActorInfo(), ActionContainer.TargetSelector, Level, this, nullptr, false);
				}
			}
			
			// 按顺序播放蒙太奇, 蓝图中执行
			if (EventID2MontageActionPointsMap.Contains(EventID))
			{
				WithMontageEventIDs.Add(EventID);
			}
		} 
	}

	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
	ContextSystem->RegisterGameplayEventWaiting(GetAvatarActorFromActorInfo());

	return WithMontageEventIDs;
}

void UTheOneDataDrivePassiveGA::ExecuteMontageAction(int32 InEventID, FGameplayTag InActionPoint)
{
	check(EventID2MontageActionPointsMap.Contains(InEventID));
	for (auto& ActionPoint : EventID2MontageActionPointsMap[InEventID])
	{
		if (ActionPoint.ActionPoint == InActionPoint)
		{
			auto Executor = GetAvatarActorFromActorInfo();
			for (FTheOneAbilityActionContainer& Action : ActionPoint.Actions)
			{
				for (FTheOneAbilityAction& ActionData : Action.Actions)
				{
					ActionData.DoAction(Executor, Action.TargetSelector, Level, this, nullptr, false);
				}
			}
		}
	}
}

void UTheOneDataDrivePassiveGA::ExecuteOver()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
	ContextSystem->UnRegisterGameplayEventWaiting(GetAvatarActorFromActorInfo());
}
