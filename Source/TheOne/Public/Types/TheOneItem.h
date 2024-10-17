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
	Equipment,
	Minion
};

UENUM(BlueprintType)
enum class ETheOnePropType : uint8
{
	Weapon,
	Shield,
	Cloth,
	Head,
	Jewelry,
	Consumable,
};

UENUM(BlueprintType)
enum class ETheOneWeaponType : uint8
{
	OneHandMelee,
	TwoHandMelee,
	OneHandRange,
	TwoHandRange,
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
// 1、装备， 功能通过配置实现， 目前包括：给与属性、给与技能等
// 2、物品， 商品，材料，任务道具， Todo: 
// 3、队伍成员

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

// Todo: 部分属性使用KV赋予，如双手、单手； 主副手持握允许
USTRUCT(BlueprintType)
struct FTheOneEquipmentConfig : public FTheOneItemBase
{
	GENERATED_BODY()

	FTheOneEquipmentConfig(): PropType(), WeaponType()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOnePropType PropType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTheOneAttrModifier> AttrModifiers;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTheOneAdditionMesh> AdditionMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (RowType = "/Script/TheOne.TheOneCharacterAnimationConfig"))
	FDataTableRowHandle AnimBPConfigRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTheOneAbilityRow> Abilities;
};

USTRUCT(BlueprintType)
struct FTheOneMinionData
{
	GENERATED_BODY()

	FTheOneMinionData()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CharacterConfigRowName;
};

// Todo: 改成KV的形式吗？
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
