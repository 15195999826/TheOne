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
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const auto NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			if (NewHealth <= 0.f)
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
