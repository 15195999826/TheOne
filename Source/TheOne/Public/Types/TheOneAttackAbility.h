#pragma once
#include "CoreMinimal.h"
#include "TheOneProjectileConfig.h"
#include "TheOneAttackAbility.generated.h"

class ATheOneProjectileActor;

UENUM(BlueprintType)
enum class ETheOneAttackCapability : uint8
{
	None,
	Melee,
	Range
};

UENUM(BlueprintType)
enum class ETheOneDamageType : uint8
{
	Physical UMETA(DisplayName="物理"),
	Magical UMETA(DisplayName="魔法"),
	Real UMETA(DisplayName="真实")
};

USTRUCT(BlueprintType)
struct  FTheOneAttackAbility
{
	GENERATED_BODY()

	FTheOneAttackAbility(): Attack(0), AttackRange(0), AttackSpeed(0), AttackCapability(), DamageType(),
	                      ProjectileSpeed(0),
	                      ProjectileFlyType(),
	                      ProjectileSpawnOffset(FVector::ZeroVector)
	{
	}

	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击力"))
	float Attack;

	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击范围"))
	int AttackRange;
	
	// 攻速 = 每秒执行几次普通动画
	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击速度"))
	float AttackSpeed;

	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击动画"))
	TSoftObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditAnywhere,meta=(DisplayName="攻击点时间"))
	TArray<float> AttackPointSeconds;

	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击能力"))
	ETheOneAttackCapability AttackCapability;

	UPROPERTY(EditAnywhere,meta=(DisplayName="伤害类型"))
	ETheOneDamageType DamageType;

	UPROPERTY(EditAnywhere, meta=(RowType = "/Script/TheOne.TheOneProjectileConfig", DisplayName="投射物类型", EditConditionHides, EditCondition="AttackCapability == ETheOneAttackCapability::Range"))
	FDataTableRowHandle ProjectileType;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere)
	ETheOneProjectileFlyType ProjectileFlyType;

	UPROPERTY(EditAnywhere)
	FName ProjectileSpawnSocketName;
	
	UPROPERTY(EditAnywhere)
	FVector ProjectileSpawnOffset;
};