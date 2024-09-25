// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TheOneAttributeAccessor.h" 
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "TheOneAttributeSet.generated.h"


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;
	
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;
	
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

USTRUCT()
struct FTheOneAttributeSetArchive
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> AttributeNames;

	UPROPERTY()
	TArray<float> AttributeValues; // 只保存BaseValue
};

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTheOneAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	FTheOneAttributeSetArchive SaveRuntimeAttributes() const;
	void LoadRuntimeAttributes(const FTheOneAttributeSetArchive& Archive);

	/*
	 * 运行时属性， 需要被保存和读取
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Level);

	/*
	 * 基础属性， 模板 + 等级修正 + GE修正
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attribute")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Attack);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attribute")
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, AttackRange);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attribute")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, AttackSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Armor; // 护甲
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MagicResistance; // 魔抗
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MagicResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MoveSpeed; // 攻速
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData TurnSpeed; // 转身速度
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, TurnSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData ProjectileSpeedMulMod; // 投射物速度修正
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, ProjectileSpeedMulMod);
	
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void AddAttributeToArchive(const FGameplayAttribute& Attribute, FTheOneAttributeSetArchive& Archive) const;
	void ApplyAttributeFromArchive(const FGameplayAttribute& Attribute, const FTheOneAttributeSetArchive& Archive);
};
