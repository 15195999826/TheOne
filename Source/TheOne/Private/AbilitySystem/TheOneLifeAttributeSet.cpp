// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TheOneLifeAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "TheOneBattleInterface.h"

void UTheOneLifeAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetBodyIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetBodyIncomingDamage();
		if (LocalIncomingDamage > 0.f)
		{
			const auto NewBodyArmor = GetBodyArmor() - LocalIncomingDamage;
			SetBodyArmor(FMath::Clamp(NewBodyArmor, 0.f, GetMaxBodyArmor()));

			if (GetBodyArmor() <= 0)
			{
				// Todo: 发出身体护甲破损事件
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHeadIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetHeadIncomingDamage();
		if (LocalIncomingDamage > 0.f)
		{
			const auto NewHeadArmor = GetHeadArmor() - LocalIncomingDamage;
			SetHeadArmor(FMath::Clamp(NewHeadArmor, 0.f, GetMaxHeadArmor()));

			if (GetHeadArmor() <= 0)
			{
				// Todo: 发出头部护甲破损事件
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetInComingDamageAttribute())
	{
		const float LocalIncomingDamage = GetInComingDamage();
		if (LocalIncomingDamage > 0.f)
		{
			const auto NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			if (GetHealth() <= 0)
			{
				if (auto TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get())
				{
					if (auto BattleInterface = Cast<ITheOneBattleInterface>(TargetAvatarActor))
					{
						BattleInterface->Die();
					}
				}
			}
		}
	}
}
