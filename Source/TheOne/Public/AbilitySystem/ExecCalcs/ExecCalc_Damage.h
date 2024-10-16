// Copyright Ogopogo

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

THEONE_API DECLARE_LOG_CATEGORY_EXTERN(LogTheOneDamage, Log, All);
/**
 * 
 */
UCLASS()
class THEONE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();
	static void CalcDamage(bool IsCalcByATK,
	                       // 用于损毁护甲时计算的额参数
	                       float MaxAttack, float OriginDamage,
	                       float DamageArmorEfficiency, float DamagePenetrationEfficiency, float TargetArmor,
	                       float MeleeDamage, float RangeDamage,
	                       float& HealthDamage, float& RealBodyArmorDamage);

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
