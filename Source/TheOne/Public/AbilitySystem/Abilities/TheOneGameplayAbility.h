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

USTRUCT(BlueprintType)
struct FTheOneAbilityConfig: public FTableRowBase
{
	GENERATED_BODY()

	FTheOneAbilityConfig(): TargetType(), ReleaseRule()
	{
		auto DTSettings = GetDefault<UTheOneDataTableSettings>();
		if (DTSettings)
		{
			AbilityClass = DTSettings->DefaultTableAbility;
		}
		else
		{
			AbilityClass = nullptr;
		}
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
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTheOneGameplayAbility> AbilityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> AbilityMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAbilityType AbilityType = ETheOneAbilityType::Active;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAbilityReleaseTarget TargetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "行动点消耗"))
	int Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "施法范围"))
	int ReleaseDistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETheOneAbilityReleaseRule ReleaseRule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition, EditConditionHides = "AbilitySource == ETheOneAbilitySource::DataTableDrive"))
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
