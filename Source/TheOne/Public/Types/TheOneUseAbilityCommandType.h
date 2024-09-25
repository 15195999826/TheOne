#pragma once

#include "CoreMinimal.h"
#include "TheOneUseAbilityCommandType.generated.h"

UENUM(BlueprintType)
enum class ETheOneUseAbilityCommandType : uint8
{
	Invalid,
	UseItem,
	UseWeaponAbility,
};
