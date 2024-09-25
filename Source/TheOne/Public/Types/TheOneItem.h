#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TheOneAdditionMesh.h"
#include "TheOneAttackAbility.h"
#include "TheOneAttrModifier.h"

#include "TheOneItem.generated.h"

UENUM(BlueprintType)
enum class ETheOneItemType : uint8
{
	None,
	Weapon,
	Prop,
};

USTRUCT(BlueprintType)
struct FTheOneAbilityRow
{
	GENERATED_BODY()

	FTheOneAbilityRow()
	{
		RowName = NAME_None;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName;
};

// 两种类型道具
// 1、武器， 职业概念的包装， 提供白字属性（定义为基础属性）， 提供普攻和技能； 可进阶， 提供性能更好的普攻和技能
// 2、物品， 提供角色属性（绿字属性，定义为附加属性）和机制，最多携带一个被动技能和一个主动技能；可合成

USTRUCT(BlueprintType)
struct FTheOneItemBase: public FTableRowBase
{
	GENERATED_BODY()

	FTheOneItemBase()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};

// Todo: 不同类型的生物配置不放入同一张表格， 一类生物应该有一组配置表：目前： CharacterConfig, CharacterAnimationConfig, CharacterWeaponConfig
// Todo: 相同武器有不同强化方向，因此每把武器都是独立的实例
USTRUCT(BlueprintType)
struct FTheOneWeaponConfig : public FTheOneItemBase
{
	GENERATED_BODY()

	FTheOneWeaponConfig()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTheOneAdditionMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (RowType = "/Script/TheOne.TheOneCharacterAnimationConfig"))
	FDataTableRowHandle AnimBPConfigRow;

	UPROPERTY(EditAnywhere)
	FTheOneAttackAbility AttackAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow PassiveAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow AbilityA;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow AbilityB;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow AbilityC;
};



USTRUCT(BlueprintType)
struct FTheOnePropConfig : public FTheOneItemBase
{
	GENERATED_BODY()

	FTheOnePropConfig()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTheOneAttrModifier> AttrModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow PassiveAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTheOneAbilityRow ActiveAbility;
};
