// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/TheOneAbilityAction.h"
#include "Development/TheOneDataTableSettings.h"
#include "TheOneGameplayAbility.generated.h"

class ATheOneCharacterBase;

UENUM(BlueprintType)
enum class ETheOneAbilitySource : uint8
{
	DataTableDrive UMETA(DisplayName = "数据表驱动"),
	SpecialBP UMETA(DisplayName = "特定BP驱动"),
	LuaDrive UMETA(DisplayName = "Lua驱动")
};

UENUM(BlueprintType)
enum class ETheOneAbilityType : uint8
{
	Active UMETA(DisplayName = "主动技能"),
	Passive UMETA(DisplayName = "被动技能")
};

UENUM(BlueprintType)
enum class ETheOneAbilityReleaseTarget : uint8
{
	None UMETA(DisplayName = "无目标"),
	Enemy UMETA(DisplayName = "一个敌人"),
	Ally UMETA(DisplayName = "一个友军(包括自己)"),
	Grid UMETA(DisplayName = "格子"),
	AnyActor UMETA(DisplayName = "任意一个单位"),
};

UENUM(BlueprintType)
enum class ETheOneAbilityReleaseRule : uint8
{
	UseOwnerReleaseRange UMETA(DisplayName = "满足释放范围"),
	Always UMETA(DisplayName = "总能释放"),
};

/////// Ability Structs Start ///////

/**
 * 用于表达一个主动技能如何释放
 */
USTRUCT(BlueprintType)
struct FTheOneActiveAbilityData
{
	GENERATED_BODY()

	FTheOneActiveAbilityData(): TargetType()
	{
	}

	// For 主动技能
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAbilityReleaseTarget TargetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "施法范围"))
	int ReleaseDistance = 0;
};

/**
 * 用于表达一个被动技能如何触发
 * Todo: 以及触发时执行的无需动画的功能
 */
USTRUCT(BlueprintType)
struct FTheOnePassiveAbilityData
{
	GENERATED_BODY()

	FTheOnePassiveAbilityData()
	{
	}

	UPROPERTY(EditAnywhere, meta=(DisplayName = "被动技能触发点"))
	TArray<FGameplayTag> TriggerActionPoint;

	// Todo: 其它配置
	
};

USTRUCT(BlueprintType)
struct FTheOneAbilityConfig: public FTableRowBase
{
	GENERATED_BODY()

	FTheOneAbilityConfig()
	{
	}

	UPROPERTY(EditAnywhere)
	FText AbilityName;

	UPROPERTY(EditAnywhere, meta=(MultiLine = true))
	FText AbilityDescription;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere)
	ETheOneAbilitySource AbilitySource = ETheOneAbilitySource::DataTableDrive;

	UPROPERTY(EditAnywhere)
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCancelOnTargetDead = false;

	/**
	 * 数据驱动的被动技能， 不会赋予AbilityClass
	 * 数据驱动的主动技能， 会赋予DefaultTableAbility
	 * 
	 * 特定BP驱动的技能， 无论主动还是被动， 都会赋予AbilityClass
	 *
	 * lua驱动的技能， 无论主动还是被动， 都会赋予AbilityClass
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition = "AbilitySource == ETheOneAbilitySource::SpecialBP || AbilitySource == ETheOneAbilitySource::LuaDrive", EditConditionHides))
	TSubclassOf<UTheOneGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(DisplayName = "评估函数名"))
	FName EvaluateFunctionName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> AbilityMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAbilityType AbilityType = ETheOneAbilityType::Active;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "行动点消耗"))
	int Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "体力消耗"))
	int EnergyCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "冷却"))
	int CoolDown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "主动技能配置", EditConditionHides, EditCondition = "AbilityType == ETheOneAbilityType::Active&&AbilitySource == ETheOneAbilitySource::DataTableDrive"))
	FTheOneActiveAbilityData ActiveAbilityData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "被动技能配置", EditConditionHides, EditCondition = "AbilityType == ETheOneAbilityType::Passive&&AbilitySource == ETheOneAbilitySource::DataTableDrive"))
	FTheOnePassiveAbilityData PassiveAbilityData;

	// 仅用于带有动画的技能，用于触发动画节点的功能 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "动画触发点功能", EditConditionHides, EditCondition = "AbilitySource == ETheOneAbilitySource::DataTableDrive"))
	TArray<FTheOneActionPointConfig> ActionPoints;
};

/////// Ability Structs End ///////
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTheOneGameplayAbility();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTheOneAbilityAfterEndSignature, bool, WasCancelled);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	UPROPERTY(BlueprintAssignable)
	FOnTheOneAbilityAfterEndSignature OnceOnTheOneGAAfterEnd;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AbilityMontage;
protected:
	int Level = 1;

	ETheOneAbilityType AbilityType = ETheOneAbilityType::Active;

public:

	ETheOneAbilityType GetAbilityType() const
	{
		return AbilityType;
	}

	int GetLevel() const
	{
		return Level;
	}

protected:
	void SetupAbility(ETheOneAbilityType InAbilityType, int InLevel, UAnimMontage* InMontage);
};
