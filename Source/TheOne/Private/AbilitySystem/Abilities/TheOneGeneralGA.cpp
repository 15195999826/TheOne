// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneGeneralGA.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"

void UTheOneGeneralGA::SetUpGeneralAbility(const FDataTableRowHandle& InRow, const FTheOneAbilityConfig* InAbilityConfig, int InLevel)
{
	AbilityRow = InRow;
	SetupAbility(InAbilityConfig->AbilityType, InLevel, InAbilityConfig->AbilityMontage.LoadSynchronous());
	for (auto& ActionPoint : InAbilityConfig->ActionPoints)
	{
		ActionPoints.Add(UTheOneBlueprintFunctionLibrary::CastConfigToAbilityActionPoint(ActionPoint));
	}
}

ETheOneAbilityReleaseTarget UTheOneGeneralGA::GetTargetType() const
{
	auto Config = AbilityRow.GetRow<FTheOneAbilityConfig>("GetTargetType");
	if (Config)
	{
		return Config->ActiveAbilityData.TargetType;
	}
	UE_LOG(LogTheOne, Error, TEXT("AbilityConfig is nullptr"));
	return ETheOneAbilityReleaseTarget::Enemy;
}

void UTheOneGeneralGA::DoAction(FGameplayTag InActionPoint)
{
	bool DrawDebug = false;
#if WITH_EDITOR
	auto Config = AbilityRow.GetRow<FTheOneAbilityConfig>("DoAction");
	DrawDebug = Config->DrawDebug;
#endif
	
	auto Executor = GetAvatarActorFromActorInfo();
	for (auto& ActionPoint : ActionPoints)
	{
		if (ActionPoint.ActionPoint == InActionPoint)
		{
			// 执行技能
			for (FTheOneAbilityActionContainer& Action : ActionPoint.Actions)
			{
				for (FTheOneAbilityAction& ActionData : Action.Actions)
				{
					// 执行Action
					ActionData.DoAction(Executor, Action.TargetSelector, Level, this, nullptr, DrawDebug);
				}
			}
		}
	}
}
