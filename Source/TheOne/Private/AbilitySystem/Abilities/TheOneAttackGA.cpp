// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneAttackGA.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneGameplayTags.h"

void UTheOneAttackGA::SetupAttackAbility(const FTheOneAttackAbility& InAttackAbility)
{
	auto Montage = InAttackAbility.AttackMontage.LoadSynchronous();
	SetupAbility(ETheOneAbilityType::Active, 1, Montage);
	
	// 蒙太奇播放rate为1时的动画时长， 将该值设置为播放Rate， 可以将动画时长变为1s
	auto DefaultLength = Montage->GetPlayLength() / (1 * Montage->RateScale);
	NormalizeRate = DefaultLength;

	for (auto& AttackPoint : InAttackAbility.AttackPointSeconds)
	{
		AttackPointPercents.Add(AttackPoint);
	}

	// Todo: 这样的话，普攻是AOE等情况也都可以实现了


	switch (InAttackAbility.AttackCapability) {
		case ETheOneAttackCapability::None:
			break;
		case ETheOneAttackCapability::Melee:
			{
				FTheOneAbilityActionContainer BuildActionContainer;
				BuildActionContainer.TargetSelector.TargetSelectorType = ETheOneTargetSelectorType::BBTarget;

				FTheOneAbilityAction Action;
				FTheOneAbilityActionData ActionData;

				ActionData.ActionType = ETheOneAbilityActionType::Damage;
				ActionData.DamageTag = UTheOneBlueprintFunctionLibrary::CastToDamageTypeTag(InAttackAbility.DamageType);
				ActionData.Expressions.Add(TEXT("ATK"));
				
				Action.ActionData = ActionData;
				BuildActionContainer.Actions.Add(Action);

				ActionContainer = BuildActionContainer;
			}
			break;
		case ETheOneAttackCapability::Range:
			{
				// AttackPoint 执行SpawnProjectile, 并且SpawnProjectile的命中功能为造成伤害
				FTheOneAbilityActionContainer BuildActionContainer;
				BuildActionContainer.TargetSelector.TargetSelectorType = ETheOneTargetSelectorType::BBTarget;
		
				FTheOneAbilityAction Action;
				FTheOneAbilityActionData ActionData;
				ActionData.ActionType = ETheOneAbilityActionType::SpawnProjectile;
				ActionData.SpawnSocketName = InAttackAbility.ProjectileSpawnSocketName;
				ActionData.SpawnOffset = InAttackAbility.ProjectileSpawnOffset;
				ActionData.ProjectileRowName = InAttackAbility.ProjectileType.RowName;
				ActionData.FlyType = InAttackAbility.ProjectileFlyType;
				ActionData.FlySpeed = InAttackAbility.ProjectileSpeed;
				FString HitActionKey = TEXT("RangeAttack");
				ActionData.HitActionKeys.Add(HitActionKey);

				FTheOneAbilityActionData HitActionData;
				HitActionData.ActionType = ETheOneAbilityActionType::Damage;
				HitActionData.DamageTag = UTheOneBlueprintFunctionLibrary::CastToDamageTypeTag(InAttackAbility.DamageType);
				HitActionData.Expressions.Add(TEXT("ATK"));

				Action.ActionData = ActionData;
				Action.ForBindActionParam.Add(HitActionKey, HitActionData);

				BuildActionContainer.Actions.Add(Action);

				ActionContainer = BuildActionContainer;
			}
			break;
	}
}

void UTheOneAttackGA::NativeOnAttackPoint(bool bIsLastAttackPoint)
{
	OnAttackPointReached.Broadcast(bIsLastAttackPoint);

	// 执行Action
	if (ActionContainer.Actions.Num() > 0)
	{
		for (auto& Action : ActionContainer.Actions)
		{
			Action.DoAction(GetAvatarActorFromActorInfo(), ActionContainer.TargetSelector, 1, this);
		}
	}
}
