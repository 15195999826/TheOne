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
	FGameplayAttributeData MinAttack;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MinAttack);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attribute")
	FGameplayAttributeData MaxAttack;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MaxAttack);
	
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

// 	近战水平
// 远程水平
// 近战防御
// 远程防御
// 伤甲效率
// 穿透效率
// Max行动点
// 体力
// 负重
// 负重级别
// 速度
// 士气状态
// 意志
// 爆头率
// 视野
// 年龄
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MeleeLevel; // 近战水平
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MeleeLevel);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData RangedLevel; // 远程水平
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, RangedLevel);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MeleeDefense; // 近战防御
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MeleeDefense);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData RangedDefense; // 远程防御
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, RangedDefense);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData DamageArmorEfficiency; // 伤甲效率
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, DamageArmorEfficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData DamagePenetrationEfficiency; // 穿透效率
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, DamagePenetrationEfficiency);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxActionPoint; // Max行动点
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MaxActionPoint);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxEnergy; // 体力
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MaxEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData MaxWeight; // 负重
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, MaxWeight);
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData WeightLevel; // 负重级别
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, WeightLevel);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Speed; // 速度
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Speed);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Morale; // 士气状态
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Morale);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Will; // 意志
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Will);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData HeadshotRate; // 爆头率
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, HeadshotRate);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Vision; // 视野
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Vision);

	UPROPERTY(BlueprintReadOnly, Category = "Vital Attributes")
	FGameplayAttributeData Age; // 年龄
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Age);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData ActionPoint;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, ActionPoint);

	UPROPERTY(BlueprintReadOnly, Category = "Runtime Attributes")
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UTheOneAttributeSet, Energy);
	
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void AddAttributeToArchive(const FGameplayAttribute& Attribute, FTheOneAttributeSetArchive& Archive) const;
	void ApplyAttributeFromArchive(const FGameplayAttribute& Attribute, const FTheOneAttributeSetArchive& Archive);
};
