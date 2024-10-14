#pragma once
#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Types/TheOneProjectileConfig.h"
#include "TheOneAbilityAction.generated.h"

class UAbilitySystemComponent;
class UTheOneGameplayAbility;
class ATheOneCharacterBase;
enum class ETheOneCamp : uint8;

UENUM(BlueprintType)
enum class ETheOneAbilityActionType : uint8
{
	Invalid,
	Damage, // 基于攻击力造成伤害
	SpawnProjectile, // 生成
	AddBuff
	
};
UENUM()
enum class ETheOneTargetSelectorType
{
	Invalid,
	BBTarget UMETA(DisplayName = "AI黑板目标"),
	TriggerActor UMETA(DisplayName = "触发被动的目标"),
	HitBox UMETA(DisplayName = "HitBox"),
	FromInterface UMETA(DisplayName = "通过执行者接口获取"), // 执行该Action的Actor实现接口ITheOneHitTarget
	Special UMETA(DisplayName = "特殊"),
};

UENUM()
enum class ETheOneHitBoxPointType
{
	Invalid,
	Caster,
	Target,
	TargetGrid
};

UENUM()
enum class ETheOneHiBoxType
{
	Invalid,
	Box,
	Circle,
	Sector UMETA(DisplayName = "扇形"),
};

USTRUCT(BlueprintType)
struct FTheOneHiBox
{
	GENERATED_BODY()

	FTheOneHiBox(): HitBoxType(ETheOneHiBoxType::Box), PointType(ETheOneHitBoxPointType::Caster)
	{
	}

	UPROPERTY(EditAnywhere)
	ETheOneHiBoxType HitBoxType;

	UPROPERTY(EditAnywhere)
	ETheOneHitBoxPointType PointType;

	UPROPERTY(EditAnywhere)
	bool HitMultiple = true;

	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "HitBoxType == ETheOneHiBoxType::Box"))
	FVector BoxSize = FVector(50, 50, 50);

	// Sector
	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "HitBoxType == ETheOneHiBoxType::Sector"))
	float SectorAngle = 90;

	// Sector or Circle
	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "HitBoxType == ETheOneHiBoxType::Sector || HitBoxType == ETheOneHiBoxType::Circle"))
	float Radius = 100;
	
	// AboveGround
	UPROPERTY(EditAnywhere)
	float AboveGround = 50;
	UPROPERTY(EditAnywhere)
	float StartDistance = 50;
	UPROPERTY(EditAnywhere)
	float EndDistance = 100;
	UPROPERTY(EditAnywhere)
	bool IgnoreSelf = true;
};


USTRUCT(BlueprintType)
struct FTheOneAbilityTargetSelector
{
	// Special, 根据String， 使用特定的函数
	
	GENERATED_BODY()

	FTheOneAbilityTargetSelector(): TargetSelectorType(ETheOneTargetSelectorType::BBTarget), SpecialSelector("")
	{
	}

	UPROPERTY(EditAnywhere)
	ETheOneTargetSelectorType TargetSelectorType;

	// Todo: 尚未使用
	UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="TargetSelectorType == ETheOneTargetSelectorType::HitBox"))
	TArray<ETheOneCamp> HitCamps;

	UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="TargetSelectorType == ETheOneTargetSelectorType::HitBox"))
	TArray<FTheOneHiBox> HiBoxes;

	UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="TargetSelectorType == ETheOneTargetSelectorType::Special"))
	FString SpecialSelector;
};

USTRUCT(BlueprintType)
struct FTheOneAbilityActionDataConfig
{
	GENERATED_BODY()

	inline static FName ExpressionKey = FName("Expression");
	inline static FName DamageTagKey = FName("DamageTag");
	inline static FName ProjectileRowNameKey = FName("ProjectileRowName");
	inline static FName SpawnSocketNameKey = FName("SpawnSocketName");
	inline static FName SpawnOffsetKey = FName("SpawnOffSet");
	inline static FName FlySpeedKey = FName("FlySpeed");
	inline static FName FlyTypeKey = FName("FlyType");
	inline static FName HitActionsKey = FName("HitActions");
	inline static FName BuffRowNameKey = FName("BuffRowName");
	
    FTheOneAbilityActionDataConfig(): ActionType(ETheOneAbilityActionType::Invalid)
    {
    }

    UPROPERTY(EditAnywhere)
    ETheOneAbilityActionType ActionType;
	
	// UPROPERTY(EditAnywhere, meta=(GetKeyOptions="TheOne.TheOneStructDeveloperSettings.GetCustomKVsValues"))
	UPROPERTY(EditAnywhere)
	TMap<FName, FName> CustomKVs;
};

USTRUCT(BlueprintType)
struct FTheOneAbilityActionConfig
{
	GENERATED_BODY()

	FTheOneAbilityActionConfig()
	{
	}

	UPROPERTY(EditAnywhere)
	FTheOneAbilityActionDataConfig ActionData;
	/**
	 * 用于绑定自定义Action的ActionPoint
	 * 使用方式： 例如 创建投射物的Action, 然后增加一个Value对应ForBindActions的 FString Key的FTheOneKV 来读取绑定的Action
	 */
	UPROPERTY(EditAnywhere)
	TMap<FString, FTheOneAbilityActionDataConfig> ForBindActionParam;
};


/**
 * 执行Action的基层容器
 */
USTRUCT(BlueprintType)
struct FTheOneAbilityActionContainerConfig
{
	GENERATED_BODY()
	 
	UPROPERTY(EditAnywhere, meta=(DisplayName="目标选择器"))
	FTheOneAbilityTargetSelector TargetSelector;

	UPROPERTY(EditAnywhere)
	TArray<FTheOneAbilityActionConfig> Actions;
};

/**
 * 当ActionPoint触发时，执行Action
 */
USTRUCT(BlueprintType)
struct FTheOneActionPointConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag ActionPoint;
	
	UPROPERTY(EditAnywhere)
	TArray<FTheOneAbilityActionContainerConfig> Actions;
};


////////////////////////// 运行时Action数据 Start ////////////////////////////////

/**
 * 运行时，执行一次，将FTheOneAbilityAction的字符数据转化为具体的值类型
 */
USTRUCT(BlueprintType)
struct FTheOneAbilityActionData
{
	GENERATED_BODY()
	
	FTheOneAbilityActionData(): ActionType(ETheOneAbilityActionType::Invalid), SpawnOffset(FVector::ZeroVector), FlySpeed(0), FlyType(ETheOneProjectileFlyType::Invalid)
	{
	}

	UPROPERTY()
	ETheOneAbilityActionType ActionType;

	// 对应不同等级的伤害表达式
	UPROPERTY()
	TArray<FString> Expressions;

	UPROPERTY()
	FGameplayTag DamageTag;

	UPROPERTY()
	FName ProjectileRowName;
	
	UPROPERTY()
	FVector SpawnOffset;

	UPROPERTY()
	FName SpawnSocketName;
	
	UPROPERTY()
	FName BuffRowName;

	UPROPERTY()
	float FlySpeed;

	UPROPERTY()
	ETheOneProjectileFlyType FlyType;

	UPROPERTY()
	TArray<FString> HitActionKeys;
};

USTRUCT(BlueprintType)
struct FTheOneAbilityAction
{
	GENERATED_BODY()

	UPROPERTY()
	FTheOneAbilityActionData ActionData;
	
	UPROPERTY()
	TMap<FString, FTheOneAbilityActionData> ForBindActionParam;
	
	static float RequireActionMathExpression(const FString& MathExpression, const AActor* ActionExecutor, const AActor* TargetActor,
										 const FActiveGameplayEffect* InSourceGE = nullptr);
	
	void DoAction(AActor* ActionExecutor, const FTheOneAbilityTargetSelector& InTargetSelector, int Level, UTheOneGameplayAbility* FromAbility = nullptr, FActiveGameplayEffect* ActionSourceGE = nullptr, bool DrawDebug = false);
private:
	/**
	 * @brief 造成伤害
	 * @param SourceActor 伤害来源, 目前，对于实现了ITheOneCharacterSpawnActor接口的Actor，会调用GetSourceCharacter获取真正的来源， 比如投射物
	 * @param TargetActor 伤害目标
	 * @param InDamage 伤害值
	 * @param FromAbility 伤害来源的技能
	 */
	void DoActionDamageInternal(AActor* SourceActor, AActor* TargetActor,float InDamage, const UTheOneGameplayAbility* FromAbility) const;


	TArray<AActor*> GetTargets(AActor* ActionExecutor, const FTheOneAbilityTargetSelector& InTargetSelector, UTheOneGameplayAbility* FromAbility, bool DrawDebug);


};

USTRUCT(BlueprintType)
struct FTheOneAbilityActionContainer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(DisplayName="目标选择器"))
	FTheOneAbilityTargetSelector TargetSelector;

	UPROPERTY(EditAnywhere)
	TArray<FTheOneAbilityAction> Actions;
};


USTRUCT(BlueprintType)
struct FTheOneAbilityActionPoint
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag ActionPoint;
	
	UPROPERTY(EditAnywhere)
	TArray<FTheOneAbilityActionContainer> Actions;
};

/////////////////////////////// 运行时Action数据 End ////////////////////////////////////