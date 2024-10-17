// Copyright Ogopogo


#include "AbilitySystem/ExecCalcs//ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "TheOneGameplayTags.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
DEFINE_LOG_CATEGORY(LogTheOneDamage);

struct TheOneSourceDamageStatics
{
	// Todo: 如果技能上以后增加了这两个参数的修正值，通过给DamageGE Assign对应Tag的参数来实现
	// 最大攻击力
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxAttack);
	// 伤甲效率
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageArmorEfficiency);
	// 穿透率
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamagePenetrationEfficiency);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MeleeLevel);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RangedLevel);
	
	TheOneSourceDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, MaxAttack, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, DamageArmorEfficiency, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, DamagePenetrationEfficiency, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, MeleeLevel, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, RangedLevel, Source, false);

	}
};

struct TheOneTargetDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(HeadArmor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BodyArmor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MeleeDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RangedDefense);
	
	TheOneTargetDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneLifeAttributeSet, HeadArmor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneLifeAttributeSet, BodyArmor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, MeleeDefense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UTheOneAttributeSet, RangedDefense, Target, false);
	}
};

static const TheOneSourceDamageStatics& SourceDamageStatics()
{
	static TheOneSourceDamageStatics Statics;
	return Statics;
}

static const TheOneTargetDamageStatics& TargetDamageStatics()
{
	static TheOneTargetDamageStatics Statics;
	return Statics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(SourceDamageStatics().MaxAttackDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().DamageArmorEfficiencyDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().DamagePenetrationEfficiencyDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MeleeLevelDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().RangedLevelDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().HeadArmorDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().BodyArmorDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().MeleeDefenseDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().RangedDefenseDef);
}

void UExecCalc_Damage::CalcDamage(bool IsCalcByATK,
                                  float MaxAttack, float OriginDamage,
                                  float DamageArmorEfficiency, float DamagePenetrationEfficiency,
                                  float TargetArmor, float MeleeDamage, float RangeDamage, float& HealthDamage,
                                  float& RealBodyArmorDamage)
{
	auto DesiredBodyArmorDamage = (MeleeDamage + RangeDamage) * DamageArmorEfficiency;
	auto DesiredRemainArmor = TargetArmor - DesiredBodyArmorDamage;
	float DesiredHealthDamage = (MeleeDamage + RangeDamage) * DamagePenetrationEfficiency - (DesiredRemainArmor > 0.f ? DesiredRemainArmor * 0.1f : 0.f);
	// 减去护甲可能出现负值， 进行保护
	if (DesiredHealthDamage < 0.f)
	{
		DesiredHealthDamage = 0.f;
	}
	RealBodyArmorDamage = DesiredBodyArmorDamage;
	float ExtraDamage = 0.f;
	if (DesiredRemainArmor <= 0.f)
	{
		if (IsCalcByATK)
		{
			ExtraDamage = MaxAttack * (1 - OriginDamage / MaxAttack);
		}

		RealBodyArmorDamage = TargetArmor;
	}

	HealthDamage = DesiredHealthDamage + ExtraDamage;

	UE_LOG(LogTheOneDamage, Verbose,
	       TEXT("CalcDamage 期望造成的护甲伤害: %f, 当前护甲: %f, 实际护甲伤害: %f, 生命值伤害: %f + (%f) = %f"),
	       DesiredBodyArmorDamage, TargetArmor, RealBodyArmorDamage, DesiredHealthDamage, ExtraDamage, HealthDamage);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UE_LOG(LogTheOneDamage, Verbose, TEXT("-------Start ExecCalc_Damage-------"));
	// 伤害计算流程
	const auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	auto SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	auto TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	UE_LOG(LogTheOneDamage, Verbose, TEXT("ExecCalc_Damage SourceActor: %s, TargetActor: %s"), *SourceActor->GetName(), *TargetActor->GetName());
	
	const auto Spec = ExecutionParams.GetOwningSpec();
	const auto SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const auto TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FGameplayTagContainer Container = Spec.GetDynamicAssetTags();
	bool IsCalcByATK = Container.HasTagExact(TheOneGameplayTags::Damage_Calc_ByATK);

	// 结算上下文
	auto EffectContextHandle = Spec.GetContext();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float DamageArmorEfficiency = 0.f;
	if (IsCalcByATK)
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().DamageArmorEfficiencyDef, EvaluateParameters, DamageArmorEfficiency);
	}
	else
	{
		// Todo: 读取GE上对应Tag的数据； 增加Action配置的KV字段， 来实现该功能
		DamageArmorEfficiency = 1.f;
	}
	
	float DamagePenetrationEfficiency = 0.f;
	if (IsCalcByATK)
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().DamagePenetrationEfficiencyDef, EvaluateParameters, DamagePenetrationEfficiency);
	}
	else
	{
		// Todo: 读取GE上对应Tag的数据； 增加Action配置的KV字段， 来实现该功能
		DamagePenetrationEfficiency = 1.f;
	}
	float MaxAttack = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().MaxAttackDef, EvaluateParameters, MaxAttack);
	float MeleeLevel = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().MeleeLevelDef, EvaluateParameters, MeleeLevel);
	float RangeLevel = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().RangedLevelDef, EvaluateParameters, RangeLevel);
	float TargetMeleeDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().MeleeDefenseDef, EvaluateParameters, TargetMeleeDefense);
	float TargetRangeDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().RangedDefenseDef, EvaluateParameters, TargetRangeDefense);
	float TargetHeadArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().HeadArmorDef, EvaluateParameters, TargetHeadArmor);
	float TargetBodyArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().BodyArmorDef, EvaluateParameters, TargetBodyArmor);
	UE_LOG(LogTheOneDamage, Verbose,
	       TEXT(
		       "ExecCalc_Damage 使用ATK计算：%d, DamageArmorEfficiency: %f, DamagePenetrationEfficiency: %f, MeleeLevel: %f, RangeLevel: %f, TargetMeleeDefense: %f, TargetRangeDefense: %f, TargetHeadArmor: %f, TargetBodyArmor: %f"
	       ), IsCalcByATK, DamageArmorEfficiency, DamagePenetrationEfficiency, MeleeLevel, RangeLevel,
	       TargetMeleeDefense,TargetRangeDefense, TargetHeadArmor, TargetBodyArmor);

	// 获取伤害
	float OriginMeleeDamage = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_Melee, false, 0.f);
	float OriginRangeDamage = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_Range, false,0.f);
	// 被包围时的近战水平加成
	float SurroundExtra = Spec.GetSetByCallerMagnitude(TheOneGameplayTags::SetByCaller_Damage_SurrondExtra, false, 0.f);
	// 战斗技巧调整
	// 当MeleeDT > 0.85f时， MeleeAdjust = 0.85f, 否则MeleeAdjust = MeleeDT
	// 当MeleeDT < -0.85f时， MeleeAdjust = -0.85f, 否则MeleeAdjust = MeleeDT
	float MeleeAdjust = FMath::Clamp(1.f+ (MeleeLevel + SurroundExtra - TargetMeleeDefense) / 100.f, 0.15f, 1.85f);
	float RangeAdjust = FMath::Clamp(1.f+ (RangeLevel - TargetRangeDefense) / 100.f, 0.15f, 1.85f);
	UE_LOG(LogTheOneDamage, Verbose, TEXT(
		"ExecCalc_Damage Original OriginMeleeDamage: %f, OriginRangeDamage: %f, 近战包围加成: %f, 近战技巧加成: %f, 远程技巧调整: %f"),
		OriginMeleeDamage, OriginRangeDamage, SurroundExtra, MeleeAdjust, RangeAdjust);
	float MeleeDamage = OriginMeleeDamage * MeleeAdjust;
	float RangeDamage = OriginRangeDamage * RangeAdjust;
	
	if (Container.HasTagExact(TheOneGameplayTags::SetByCaller_DamagePosition_Body))
	{
		// 造成身体伤害
		// 计算流程：
		// if 护甲值<=0,
		//	则直接造成伤害
		// else
		//	护甲值>0, 则先计算扣除的身体护甲， 为伤害值*穿甲效率， 记录期望扣除和实际扣除
		//  穿透伤害为 伤害值 * 穿透效率 - 剩余护甲值*10%
		//  if 剩余护甲值<=0
		//	   再造成额外的伤害: 伤害值 * (1 - 实际扣除/期望扣除)
		if (TargetBodyArmor <= 0.f)
		{
			UE_LOG(LogTheOneDamage, Verbose, TEXT("无身体护甲, HealthDamage: %f"), MeleeDamage + RangeDamage);
			const FGameplayModifierEvaluatedData EvaluatedData(UTheOneLifeAttributeSet::GetInComingDamageAttribute(), EGameplayModOp::Additive, MeleeDamage + RangeDamage);
			OutExecutionOutput.AddOutputModifier(EvaluatedData);
		}
		else
		{
			UE_LOG(LogTheOneDamage, Verbose, TEXT("---------有身体护甲计算伤害------"));
			float HealthDamage;
			float ArmorDamage;
			CalcDamage(IsCalcByATK, MaxAttack, OriginMeleeDamage+OriginRangeDamage, DamageArmorEfficiency, DamagePenetrationEfficiency, TargetBodyArmor, MeleeDamage, RangeDamage, HealthDamage, ArmorDamage);

			const FGameplayModifierEvaluatedData BodyArmorEvaluatedData(UTheOneLifeAttributeSet::GetBodyIncomingDamageAttribute(), EGameplayModOp::Additive, ArmorDamage);
			OutExecutionOutput.AddOutputModifier(BodyArmorEvaluatedData);

			const FGameplayModifierEvaluatedData HealthEvaluatedData(UTheOneLifeAttributeSet::GetInComingDamageAttribute(), EGameplayModOp::Additive, HealthDamage);
			OutExecutionOutput.AddOutputModifier(HealthEvaluatedData);
		}
	}
	else if (Container.HasTagExact(TheOneGameplayTags::SetByCaller_DamagePosition_Head))
	{
		if (TargetHeadArmor <= 0.f)
		{
			auto HealthDamage = (MeleeDamage + RangeDamage) * 1.5f;
			UE_LOG(LogTheOneDamage, Verbose, TEXT("无头部护甲, HealthDamage: %f"), HealthDamage);
			const FGameplayModifierEvaluatedData EvaluatedData(UTheOneLifeAttributeSet::GetInComingDamageAttribute(), EGameplayModOp::Additive, HealthDamage);
			OutExecutionOutput.AddOutputModifier(EvaluatedData);
		}
		else
		{
			UE_LOG(LogTheOneDamage, Verbose, TEXT("--------有头部护甲计算伤害------"));
			float HealthDamage;
			float ArmorDamage;
			CalcDamage(IsCalcByATK, MaxAttack, OriginMeleeDamage+OriginRangeDamage, DamageArmorEfficiency, DamagePenetrationEfficiency, TargetHeadArmor, MeleeDamage * 1.5f, RangeDamage * 1.5f, HealthDamage, ArmorDamage);
			const FGameplayModifierEvaluatedData HeadArmorEvaluatedData(UTheOneLifeAttributeSet::GetHeadIncomingDamageAttribute(), EGameplayModOp::Additive, ArmorDamage);
			OutExecutionOutput.AddOutputModifier(HeadArmorEvaluatedData);
			
			const FGameplayModifierEvaluatedData HealthEvaluatedData(UTheOneLifeAttributeSet::GetInComingDamageAttribute(), EGameplayModOp::Additive, HealthDamage);
			OutExecutionOutput.AddOutputModifier(HealthEvaluatedData);	
		}
	}
	else
	{
		UE_LOG(LogTheOne, Error, TEXT("伤害未设置位置标签, 无伤害"));
	}
	UE_LOG(LogTheOneDamage, Verbose, TEXT("-------End ExecCalc_Damage-------"));
}
