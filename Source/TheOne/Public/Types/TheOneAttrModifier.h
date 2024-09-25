#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TheOneGameplayTags.h"
#include "TheOneAttrModifier.generated.h"

UENUM(BlueprintType)
enum class ETheOneAttr : uint8
{
	None = 0,
	MaxHealth = 1,
	MaxMana = 2,
	HealthRegen = 3,
	ManaRegen = 4,
	Attack = 5,
	AttackRange = 6,
	AttackSpeed = 7,
	Armor = 8,
	MagicResistance = 9,
	MoveSpeed = 10,
	TurnSpeed = 11,
};

USTRUCT(BlueprintType)
struct FTheOneAttrModifier
{
	GENERATED_BODY()

	FTheOneAttrModifier(): AttrTag(), Value(0), bIsPercent(false)
	{
	}

	inline static TMap<ETheOneAttr, FGameplayTag> AttrTagMap = {
		{ETheOneAttr::MaxHealth, TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxHealth},
		{ETheOneAttr::MaxMana, TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxMana},
		{ETheOneAttr::HealthRegen, TheOneGameplayTags::SetByCaller_Attribute_Vital_HealthRegen},
		{ETheOneAttr::ManaRegen, TheOneGameplayTags::SetByCaller_Attribute_Vital_ManaRegen},
		{ETheOneAttr::Attack, TheOneGameplayTags::SetByCaller_Attribute_Vital_Attack},
		{ETheOneAttr::AttackRange, TheOneGameplayTags::SetByCaller_Attribute_Vital_AttackRange},
		{ETheOneAttr::AttackSpeed, TheOneGameplayTags::SetByCaller_Attribute_Vital_AttackSpeed},
		{ETheOneAttr::Armor, TheOneGameplayTags::SetByCaller_Attribute_Vital_Armor},
		{ETheOneAttr::MagicResistance, TheOneGameplayTags::SetByCaller_Attribute_Vital_MagicResistance},
		{ETheOneAttr::MoveSpeed, TheOneGameplayTags::SetByCaller_Attribute_Vital_MoveSpeed},
		{ETheOneAttr::TurnSpeed, TheOneGameplayTags::SetByCaller_Attribute_Vital_TurnSpeed},
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAttr AttrTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsPercent;
};