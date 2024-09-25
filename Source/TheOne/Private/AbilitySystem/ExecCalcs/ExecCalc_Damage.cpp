// Copyright Ogopogo


#include "AbilitySystem/ExecCalcs//ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "TheOneGameplayTags.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"

struct PanguDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	
	PanguDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, MagicResistance, Target, false);
	}
};

static const PanguDamageStatics& DamageStatics()
{
	static PanguDamageStatics Statics;
	return Statics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 伤害计算流程
	const auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	auto SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	auto TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	UE_LOG(LogTheOne, Verbose, TEXT("ExecCalc_Damage SourceActor: %s, TargetActor: %s"), *SourceActor->GetName(), *TargetActor->GetName());
	
	const auto Spec = ExecutionParams.GetOwningSpec();
	const auto SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const auto TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 结算上下文
	auto EffectContextHandle = Spec.GetContext();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 获取伤害
	float PhysicalDamage = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_Physical, false, 0.f);
	float MagicDamage = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_Magical, false,0.f);
	float RealDamage = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_Real, false, 0.f);
	UE_LOG(LogTheOne, Verbose, TEXT("ExecCalc_Damage Original PhysicalDamage: %f, MagicDamage: %f, RealDamage: %f"), PhysicalDamage, MagicDamage, RealDamage);

	// 获取防御
	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, Defense);
	UE_LOG(LogTheOne, Verbose, TEXT("ExecCalc_Damage Defense: %f"), Defense);
	if (PhysicalDamage > 0.f)
	{
		PhysicalDamage = FMath::Max(PhysicalDamage - Defense, 0.f);
	}

	// 获取魔抗
	float MagicResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicResistanceDef, EvaluateParameters, MagicResistance);
	UE_LOG(LogTheOne, Verbose, TEXT("ExecCalc_Damage MagicResistance: %f"), MagicResistance);
	if (MagicDamage > 0.f)
	{
		MagicDamage = FMath::Max(MagicDamage - MagicResistance, 0.f);
	}

	UE_LOG(LogTheOne, Verbose, TEXT("ExecCalc_Damage Final PhysicalDamage: %f, MagicDamage: %f, RealDamage: %f"), PhysicalDamage, MagicDamage, RealDamage);
	const FGameplayModifierEvaluatedData EvaluatedData(UTheOneLifeAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, PhysicalDamage + MagicDamage + RealDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}