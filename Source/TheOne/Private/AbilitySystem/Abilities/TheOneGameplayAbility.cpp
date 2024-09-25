// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/TheOneCharacterBase.h"
#include "Development/TheOneDeveloperSettings.h"

UTheOneGameplayAbility::UTheOneGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UTheOneGameplayAbility::SetupAbility(ETheOneAbilityType InAbilityType, int InLevel, UAnimMontage* InMontage)
{
	AbilityType = InAbilityType;
	Level = InLevel;
	AbilityMontage = InMontage;
}

void UTheOneGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	if (AbilityType == ETheOneAbilityType::Active){
		auto Character = Cast<ATheOneCharacterBase>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			Character->SetSkillState(ETheOneCharacterSkillState::Casting);
		}
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UTheOneGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AbilityType == ETheOneAbilityType::Active)
	{
		auto Character = Cast<ATheOneCharacterBase>(ActorInfo->AvatarActor.Get());
		if (Character)
		{
			Character->SetSkillState(ETheOneCharacterSkillState::Idle);
		}
	}
	OnceOnTheOneGAAfterEnd.Broadcast(bWasCancelled);
	OnceOnTheOneGAAfterEnd.Clear();
}

