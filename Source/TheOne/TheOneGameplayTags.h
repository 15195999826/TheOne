// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace TheOneGameplayTags
{
	THEONE_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	THEONE_API	extern const TArray<FGameplayTag> VitalAttributes;
	THEONE_API	extern const TArray<FGameplayTag> RuntimeAttributes;

	// Vital Attribute Tags
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxHealth);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MaxMana);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_HealthRegen);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_ManaRegen);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Attack);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_AttackRange);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_AttackSpeed);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_Armor);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MagicResistance);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_MoveSpeed);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Vital_TurnSpeed);

	// Runtime Attribute Tags
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Level);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Health);
	THEONE_API    UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Runtime_Mana);

	// Damage Tags
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Physical);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Magical);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Real);
	
	// ActionPoints
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint0);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint1);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint2);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint3);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint4);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint5);
	
	// Declare all of the custom native tags that Lyra will use
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);


	//
	// THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	// THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
	
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Stun);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	THEONE_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	THEONE_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	// 移动能力枚举
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Ground);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Fly);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Stand);

	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debug_GE);


	// 战斗事件
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Attack_Hit);

	// 攻击方投射物事件
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeforeHit);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Hit);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Miss);
	// 受击方投射物事件
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeforeBeHit);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_BeHit);
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_Dodge);
	
	THEONE_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Projectile_End);
};
