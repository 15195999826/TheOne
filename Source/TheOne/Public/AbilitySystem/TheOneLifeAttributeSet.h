// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "TheOneAttributeAccessor.h" 
#include "TheOneLifeAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneLifeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attribute")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, Mana);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, MaxMana);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, HealthRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, ManaRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData MaxHeadArmor;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, MaxHeadArmor);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData HeadArmor;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, HeadArmor);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData MaxBodyArmor;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, MaxBodyArmor);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData BodyArmor;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, BodyArmor);
	/*
	 * 隐藏属性
	 */
	UPROPERTY(BlueprintReadOnly, Category="Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UTheOneLifeAttributeSet, IncomingDamage);
};
