#pragma once

#include "CoreMinimal.h"
#include "TheOneAdditionMesh.h"
#include "TheOneAttackAbility.h"
#include "TheOneGameplayTags.h"
#include "Development/TheOneDataTableSettings.h"
#include "Retargeter/IKRetargeter.h"
#include "TheOneCharacterConfigStruct.generated.h"

class UGameplayEffect;
class UTheOneGameplayAbility;
class ATheOneAIController;

/**
 * 对战概念 ：诸天万界打擂台？诸国擂台赛？我是英灵召唤师？
 * 职业规划
 * 1、野兽
 * 2、职业，武器
 * 3、
 *
 * 不做转职，每一个角色技能都是固定的（模型相同，皮肤可以不同）， 通过天赋增强技能让角色选择不同的方向， 通过一些奇遇，更改角色技能（这个就稍微难做一点，因为会跟天赋增加功能发生冲突；或者返回强化点，或者也会存在升级版本）
 */

// Build数据
USTRUCT(BlueprintType)
struct FTheOneMinionBuildData
{
	GENERATED_BODY()

	// Todo: 皮肤、武器覆盖、技能数据
	
};


USTRUCT(BlueprintType)
struct FTheOneCharacterCustomTemplate
{
	GENERATED_BODY()

	FTheOneCharacterCustomTemplate(): MeshRootOffset(0 , 0,-89.f), ModelScale(1), HealthBarOffset(0, 0, 100),
	                                CollisionBoundHeight(88.0f),CollisionBoundRadius(34.0f)
	{
		MinionClass = nullptr;
		auto DTSettings = GetDefault<UTheOneDataTableSettings>();
		MinionClass = DTSettings->DefaultCharacterClass;
		AIControllerClass = DTSettings->DefaultAIControllerClass;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MinionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATheOneCharacterBase> MinionClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATheOneAIController> AIControllerClass;

	// SkeletalMesh
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SoftMainMesh;

	// AnimBlueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, meta=(DisplayName="是否使用重定向"))
	bool bIsRetarget = false;

	UPROPERTY(EditAnywhere, meta=(EditConditionHides, EditCondition="bIsRetarget"))
	TSubclassOf<UAnimInstance> RetargetAnimBP;

	// Retarget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditConditionHides, EditCondition="bIsRetarget"))
	TSoftObjectPtr<UIKRetargeter> Retargeter;

	// Material
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInterface> SoftMainMaterial;

	// AdditionalMeshes
	UPROPERTY(EditAnywhere)
	TArray<FTheOneAdditionMesh> AdditionalMeshes;

	UPROPERTY(EditAnywhere)
	FVector MeshRootOffset;

	// ModelScale
	UPROPERTY(EditAnywhere)
	float ModelScale;

	// 生命条偏移
	UPROPERTY(EditAnywhere)
	FVector HealthBarOffset;

	// 碰撞体积
	UPROPERTY(EditAnywhere)
	float CollisionBoundHeight;

	UPROPERTY(EditAnywhere)
	float CollisionBoundRadius;
};

USTRUCT(BlueprintType)
struct FTheOneAbilityArray
{
	GENERATED_BODY()

	FTheOneAbilityArray()
	{
	}
	
	// 仅用于选择技能
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	FName AbilitySelector;
#endif
	
	// Abilities
	UPROPERTY(EditAnywhere)
	TArray<FName> AbilityRows;
};

USTRUCT(BlueprintType)
struct FTheOneCharacterAttrTemplate
{
	GENERATED_BODY()

	FTheOneCharacterAttrTemplate(): MaxHealth(100), HealthRegen(0), MaxMana(100), ManaRegen(0),
	                              Armor(0),
	                              MagicResistance(0),
	                              MoveSpeed(500),
	                              TurnSpeed(720)
	{
		MoveMode = TheOneGameplayTags::Movement_Mode_Ground;
	}

	UPROPERTY(EditAnywhere, meta=(DisplayName="最大生命值"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, meta=(DisplayName="基础生命恢复"))
	float HealthRegen;

	UPROPERTY(EditAnywhere, meta=(DisplayName="最大法力值"))
	float MaxMana;
	
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础法力恢复"))
	float ManaRegen;

	// 护甲
	UPROPERTY(EditAnywhere, meta=(DisplayName="护甲"))
	float Armor;

	// 魔抗
	UPROPERTY(EditAnywhere, meta=(DisplayName="魔抗"))
	float MagicResistance;
	
	// 移动能力
	UPROPERTY(EditAnywhere, meta=(DisplayName="移动能力"))
	FGameplayTag MoveMode;

	// 移动速度
	UPROPERTY(EditAnywhere, meta=(DisplayName="移动速度"))
	float MoveSpeed;

	UPROPERTY(EditAnywhere, meta=(DisplayName="转身速度"))
	float TurnSpeed;
};

// 所有DataTableRow的命名以Config结尾
USTRUCT(BlueprintType)
struct FTheOneCharacterConfig: public FTableRowBase
{
	GENERATED_BODY()

	FTheOneCharacterConfig()
	{
		
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="常规"))
	FTheOneCharacterCustomTemplate Custom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (RowType = "/Script/TheOne.TheOneWeaponConfig", DisplayName="默认武器"))
	FDataTableRowHandle DefaultWeaponConfigRow;
	
	// 角色自身技能
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="技能"))
	FTheOneAbilityArray Ability;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="属性"))
	FTheOneCharacterAttrTemplate Attr;

	// Todo: 升级获得
};

USTRUCT(BlueprintType)
struct FTheOneCharacterAnimationConfig : public FTableRowBase
{
	GENERATED_BODY()

	FTheOneCharacterAnimationConfig()
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="Walk/Run"))
	TSoftObjectPtr<UBlendSpace> MoveBlendSpace;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimSequence> IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimSequence> DeadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimSequence> StunAnim;
};


UENUM(BlueprintType)
enum class ETheOneCamp : uint8
{
	None,
	Player,
	Enemy,
	Neutral
};


USTRUCT(BlueprintType)
struct FTheOneAIPawnSpawnInfo
{
	GENERATED_BODY()

	FTheOneAIPawnSpawnInfo(): Camp(ETheOneCamp::None)
	{
		StartTag = NAME_None;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETheOneCamp Camp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterTemplateRowName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StartTag;
};

USTRUCT(BlueprintType)
struct FTheOneLevelEnemy
{
	GENERATED_BODY()

	FTheOneLevelEnemy()
	{
		Row = 0;
		Column = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Column;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle MinionRow;
};


// Todo: 改名太麻烦了，蓝图里大量使用到了它
USTRUCT(BlueprintType)
struct FTheOneAutoChessLevelConfig : public FTableRowBase
{
	GENERATED_BODY()

	FTheOneAutoChessLevelConfig()
	{
		LevelName = NAME_None;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(JumpTableName="TheOneMinion"))
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTheOneLevelEnemy> AIInfos;
};