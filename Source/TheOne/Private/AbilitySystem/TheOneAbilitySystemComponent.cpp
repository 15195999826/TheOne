// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TheOneAbilitySystemComponent.h"

#include "TheOneGameplayTags.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "Character/TheOneCharacterBase.h"

UTheOneAbilitySystemComponent::UTheOneAbilitySystemComponent()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UTheOneAbilitySystemComponent::OnBattleGameplayEffectAppliedToSelf);
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UTheOneAbilitySystemComponent::OnBattleGameplayEffectRemoved);
}

void UTheOneAbilitySystemComponent::HandleMontagePostEvent(FGameplayTag EventTag, FTheOneMontageEventData EventData)
{
	if (FTheOneMontagePostDelegate* Delegate = MontageEventCallbacks.Find(EventTag))
	{
		// Make a copy before broadcasting to prevent memory stomping
		FTheOneMontagePostDelegate DelegateCopy = *Delegate;
		DelegateCopy.Broadcast(&EventData);
	}
}

UTheOneGameplayAbility* UTheOneAbilitySystemComponent::TryTheOneActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, ETheOneTryActiveResult& Success)
{
	// 检查角色当前状态，确定是否可以释放技能
	auto AbilitySpec = FindAbilitySpecFromHandle(AbilityToActivate);
	if (!AbilitySpec)
	{
		Success = ETheOneTryActiveResult::Failed;
		UE_LOG(LogTheOne, Error, TEXT("TryTheOneActivateAbility, AbilitySpec is nullptr"));
		return nullptr;
	}

	auto Instanced = AbilitySpec->GetPrimaryInstance();
	auto TheOneAbility = Instanced!=nullptr?Cast<UTheOneGameplayAbility>(Instanced):Cast<UTheOneGameplayAbility>(AbilitySpec->Ability);
	if (TheOneAbility == nullptr)
	{
		Success = ETheOneTryActiveResult::Failed;
		UE_LOG(LogTheOne, Error, TEXT("TryTheOneActivateAbility, Instance is nullptr"));
		return nullptr;
	}

	if (TheOneAbility->GetAbilityType() == ETheOneAbilityType::Passive)
	{
		UE_LOG(LogTheOne, Warning, TEXT("TryTheOneActivateAbility, 不应该使用该函数激活Passive Ability, AbilityName: %s"), *AbilitySpec->Ability->GetName());
		Success = ETheOneTryActiveResult::Failed;
		return nullptr;
	}
	
	if (TheOneAbility->GetAbilityType() == ETheOneAbilityType::Active)
	{
		if (ITheOneBattleInterface::Execute_GetSkillState(GetOwner())!= ETheOneCharacterSkillState::Idle)
		{
			UE_LOG(LogTheOne, Warning, TEXT("TryTheOneActivateAbility, 当前技能状态不是Idle, 不能释放技能, AbilityName: %s"), *AbilitySpec->Ability->GetName());
			Success = ETheOneTryActiveResult::Failed;
			return nullptr;
		}
	}
	
	if (TryActivateAbility(AbilityToActivate, false))
	{
		if (Instanced)
		{
			Success = ETheOneTryActiveResult::Succeed;
		} else
		{
			Success = ETheOneTryActiveResult::SucceedNoInstanced;
			UE_LOG(LogTheOne, Warning, TEXT("UTheOneAbilitySystemComponent::TryTheOneActivateAbility, Instanced Ability is nullptr, 返回Default Ability, AbilityName: %s"), *AbilitySpec->Ability->GetName());
		}
		
		return TheOneAbility;
	}
	
	UE_LOG(LogTheOne, Error, TEXT("不明错误，激活技能失败, AbilityName: %s"), *AbilitySpec->Ability->GetName());
	Success = ETheOneTryActiveResult::Failed;
	return nullptr;
}

void UTheOneAbilitySystemComponent::OnBattleGameplayEffectAppliedToSelf(UAbilitySystemComponent* AbilitySystemComponent,
                                                                      const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer Container;
	GameplayEffectSpec.GetAllAssetTags(Container);
	if (Container.HasTag(TheOneGameplayTags::Debug_GE))
	{
		UE_LOG(LogTemp, Warning, TEXT("[GetGE]Self: %s BuffName: %s"), *GetOwner()->GetName(), *GameplayEffectSpec.Def.GetName());
	}

}

void UTheOneAbilitySystemComponent::OnBattleGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	FGameplayTagContainer Container;
	ActiveGameplayEffect.Spec.GetAllAssetTags(Container);
	if (Container.HasTag(TheOneGameplayTags::Debug_GE))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RemoveGE]Self: %s, BuffName: %s"), *GetOwner()->GetName(), *ActiveGameplayEffect.Spec.Def.GetName());
	}
}
