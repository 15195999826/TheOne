// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneGeneralGA.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"

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

const FTheOneAbilityConfig* UTheOneGeneralGA::GetConfig()
{
	return AbilityRow.GetRow<FTheOneAbilityConfig>("GetConfig");
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

void UTheOneGeneralGA::PredictDamage_Implementation(AActor* Target, float& OutDamage, bool& OutMaybeDead)
{
	float TotalDamage = 0;
	
	for (auto& ActionPoint : ActionPoints)
	{
		for (FTheOneAbilityActionContainer& ActionContainer : ActionPoint.Actions)
		{
			for (FTheOneAbilityAction& Action : ActionContainer.Actions)
			{
				// 进行预测
				if (Action.ActionData.ActionType == ETheOneAbilityActionType::Damage)
				{
					FString Expression = "0";
					if (Action.ActionData.Expressions.Num() > 0)
					{
						Expression = Action.ActionData.Expressions[FMath::Clamp(Level - 1, 0, Action.ActionData.Expressions.Num() - 1)];
					}
					TotalDamage += FTheOneAbilityAction::RequireActionMathExpression(Expression, GetAvatarActorFromActorInfo(), Target, PredictRandomStream);
					// Todo: 暴击概率等属性应用
				}
			}
		}
	}

	OutDamage = TotalDamage;
	bool Find = false;
	auto CurrentHealth = UAbilitySystemBlueprintLibrary::GetFloatAttribute(Target, UTheOneLifeAttributeSet::GetHealthAttribute(), Find);
	if (Find)
	{
		OutMaybeDead = TotalDamage >= CurrentHealth;
	}
	else
	{
		OutMaybeDead = false;
	}
}
