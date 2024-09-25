// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TheOneAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AI/TheOneAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/TheOneCharacterBase.h"
#include "GameFramework/Character.h"

UTheOneAttributeSet::UTheOneAttributeSet()
{
	
}

void UTheOneAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UTheOneAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UTheOneAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UTheOneAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	// FEffectProperties Props;
	// SetEffectProperties(Data, Props);
}

FTheOneAttributeSetArchive UTheOneAttributeSet::SaveRuntimeAttributes() const
{
	FTheOneAttributeSetArchive Archive;
	// AddAttributeToArchive(GetLevelAttribute(), Archive);
	// AddAttributeToArchive(GetHealthAttribute(), Archive);
	// AddAttributeToArchive(GetManaAttribute(), Archive);
	return Archive;
}

void UTheOneAttributeSet::LoadRuntimeAttributes(const FTheOneAttributeSetArchive& Archive)
{
	// for (int i = 0; i < Archive.AttributeNames.Num(); i++)
	// {
	// 	auto Key = Archive.AttributeNames[i];
	// 	auto Value = Archive.AttributeValues[i];
	// 	if (Key == GetLevelAttribute().GetName())
	// 	{
	// 		InitLevel(Value);
	// 	}
	// 	else if (Key == GetHealthAttribute().GetName())
	// 	{
	// 		InitHealth(Value);
	// 	}
	// 	else if (Key == GetManaAttribute().GetName())
	// 	{
	// 		InitMana(Value);
	// 	}
	// }
}

void UTheOneAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = pawn->GetController();
			}
		}
		if (Props.SourceController)
		{ 
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		if (Props.TargetController == nullptr && Props.TargetAvatarActor != nullptr)
		{
			if (const APawn* pawn = Cast<APawn>(Props.TargetAvatarActor))
			{
				Props.TargetController = pawn->GetController();
			}
		}
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UTheOneAttributeSet::AddAttributeToArchive(const FGameplayAttribute& Attribute,
	FTheOneAttributeSetArchive& Archive) const
{
	Archive.AttributeNames.Add(Attribute.GetName());
	Archive.AttributeValues.Add(Attribute.GetGameplayAttributeData(this)->GetBaseValue());
}

void UTheOneAttributeSet::ApplyAttributeFromArchive(const FGameplayAttribute& Attribute,
	const FTheOneAttributeSetArchive& Archive)
{
}
