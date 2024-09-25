// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheOneGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "TheOneLogChannels.h"

namespace TheOneGameplayTags
{
	// Vital Attribute Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_MaxHealth, "SetByCaller.Attribute.Vital.MaxHealth", "The maximum health of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_MaxMana, "SetByCaller.Attribute.Vital.MaxMana", "The maximum mana of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_HealthRegen, "SetByCaller.Attribute.Vital.HealthRegen", "The health regeneration of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_ManaRegen, "SetByCaller.Attribute.Vital.ManaRegen", "The mana regeneration of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_Attack, "SetByCaller.Attribute.Vital.Attack", "The attack power of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_AttackRange, "SetByCaller.Attribute.Vital.AttackRange", "The attack range of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_AttackSpeed, "SetByCaller.Attribute.Vital.AttackSpeed", "The attack speed of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_Armor, "SetByCaller.Attribute.Vital.Armor", "The armor of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_MagicResistance, "SetByCaller.Attribute.Vital.MagicResistance", "The magic resistance of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_MoveSpeed, "SetByCaller.Attribute.Vital.MoveSpeed", "The move speed of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Vital_TurnSpeed, "SetByCaller.Attribute.Vital.TurnSpeed", "The turn speed of an entity.");

	// Runtime Attribute Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Runtime_Level, "SetByCaller.Attribute.Runtime.Level", "The level of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Runtime_Health, "SetByCaller.Attribute.Runtime.Health", "The current health of an entity.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Runtime_Mana, "SetByCaller.Attribute.Runtime.Mana", "The current mana of an entity.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Runtime_Attack, "SetByCaller.Attribute.Runtime.Attack", "The current attack power of an entity.");
	
	const TArray<FGameplayTag> VitalAttributes = {
		SetByCaller_Attribute_Vital_MaxHealth,
		SetByCaller_Attribute_Vital_MaxMana,
		SetByCaller_Attribute_Vital_HealthRegen,
		SetByCaller_Attribute_Vital_ManaRegen,
		SetByCaller_Attribute_Vital_Attack,
		SetByCaller_Attribute_Vital_AttackRange,
		SetByCaller_Attribute_Vital_AttackSpeed,
		SetByCaller_Attribute_Vital_Armor,
		SetByCaller_Attribute_Vital_MagicResistance,
		SetByCaller_Attribute_Vital_MoveSpeed,
		SetByCaller_Attribute_Vital_TurnSpeed
	};

	const TArray<FGameplayTag> RuntimeAttributes = {
		SetByCaller_Attribute_Runtime_Level,
		SetByCaller_Attribute_Runtime_Health,
		SetByCaller_Attribute_Runtime_Mana
	};

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage_Physical, "SetByCaller.Damage.Physical", "SetByCaller tag used by physical damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage_Magical, "SetByCaller.Damage.Magical", "SetByCaller tag used by magical damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage_Real, "SetByCaller.Damage.Real", "SetByCaller tag used by real damage gameplay effects.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint0, "Ability.ActionPoint0", "Action Point 0");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint1, "Ability.ActionPoint1", "Action Point 1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint2, "Ability.ActionPoint2", "Action Point 2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint3, "Ability.ActionPoint3", "Action Point 3");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint4, "Ability.ActionPoint4", "Action Point 4");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint5, "Ability.ActionPoint5", "Action Point 5");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");
	
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Stun, "Status.Stun", "Target is stunned.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Target has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");
						  
	// These are mapped to the movement modes inside GetMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Ground, "Movement.Mode.Ground", "The character is on the ground.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Fly, "Movement.Mode.Fly", "The character is flying.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Stand, "Movement.Mode.Stand", "The character is standing.");

	// When extending Lyra, you can create your own movement modes but you need to update GetCustomMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Custom, "Movement.Mode.Custom", "This is invalid and should be replaced with custom tags.  See LyraGameplayTags::CustomMovementModeTagMap.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debug_GE, "Debug.GE", "Debug tag for Gameplay Effects.");
	
	auto BattleEventChineseTag0 = TEXT("Battle.Event.普攻.命中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Attack_Hit, BattleEventChineseTag0, "普攻命中");
	auto BattleEventChineseTag1 = TEXT("Battle.Event.投射物.命中前");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_BeforeHit, BattleEventChineseTag1, "投射物命中前");
	auto BattleEventChineseTag2 = TEXT("Battle.Event.投射物.命中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_Hit, BattleEventChineseTag2, "投射物命中");
	auto BattleEventChineseTag3 = TEXT("Battle.Event.投射物.未命中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_Miss, BattleEventChineseTag3, "投射物未命中");
	auto BattleEventChineseTag4 = TEXT("Battle.Event.投射物.被命中前");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_BeforeBeHit, BattleEventChineseTag4, "投射物被命中前");
	auto BattleEventChineseTag5 = TEXT("Battle.Event.投射物.被命中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_BeHit, BattleEventChineseTag5, "投射物被命中");
	auto BattleEventChineseTag6 = TEXT("Battle.Event.投射物.闪避");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_Dodge, BattleEventChineseTag6, "投射物闪避");
	auto BattleEventChineseTag7 = TEXT("Battle.Event.投射物.结束");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Projectile_End, BattleEventChineseTag7, "投射物结束");

	
	// Unreal Movement Modes
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_NavWalking, Movement_Mode_Ground },
		{ MOVE_Flying, Movement_Mode_Fly },
		{ MOVE_None, Movement_Mode_Stand },
	};

	// Custom Movement Modes
	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		// Fill these in with your custom modes
	};

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogTheOne, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

