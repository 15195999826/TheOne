// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "NativeGameplayTags.h"

namespace TheOneGameplayTags
{
	THEONE_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	THEONE_API extern const TArray<FGameplayTag> WeaponModAttributes;
	THEONE_API extern const TArray<FGameplayTag> RuntimeAttributes;
	THEONE_API extern const TMap<FGameplayTag, FText> Attributes2Text;
	THEONE_API extern const TMap<FGameplayTag, TFunction<FGameplayAttribute()>> Tag2Attribute;

	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Calc_ByATK);
	

	// Vital Attribute Tags
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxHealth);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxMana);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_HealthRegen);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_ManaRegen);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MinAttack);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxAttack);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_AttackRange);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_AttackSpeed);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Armor);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MagicResistance);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MoveSpeed);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_TurnSpeed);

	// 近战水平
	// 远程水平
	// 近战防御
	// 远程防御
	// 伤甲效率
	// 穿透效率
	// Max行动点
	// 体力
	// 负重
	// 负重级别
	// 速度
	// 士气状态
	// 意志
	// 爆头率
	// 视野
	// 年龄
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MeleeLevel);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_RangeLevel);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MeleeDefense);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_RangeDefense);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_ArmorEfficiency);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_PenetrationEfficiency);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxActionPoint);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxEnergy);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxWeight);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_WeightLevel);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Speed);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Morale);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Will);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_HeadshotRate);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Vision);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Age);

	// Runtime Attribute Tags
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Level);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Health);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Mana);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_MaxHeadArmor);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_HeadArmor);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_MaxBodyArmor);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_BodyArmor);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_ActionPoint);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Energy);

	// Damage Position Tags
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamagePosition_Head);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamagePosition_Body);

	// Damage Tags
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Melee);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Range);

	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_SurrondExtra);

	// ActionPoints
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint0);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint1);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint2);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint3);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint4);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint5);

	// Declare all of the custom native tags that Lyra will use
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);


	//
	// THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	// THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Stun);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	THEONE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	THEONE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	// 移动能力枚举
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Ground);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Fly);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Stand);

	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debug_GE);


	// 战斗事件
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Attack_Hit);

	// 攻击方投射物事件
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeforeHit);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Hit);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Miss);
	// 受击方投射物事件
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeforeBeHit);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeHit);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Dodge);

	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_End);

	// 移动事件
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_BeforeMove);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_AfterMove);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_EnterZOC);
	THEONE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_LeaveZOC);
};
