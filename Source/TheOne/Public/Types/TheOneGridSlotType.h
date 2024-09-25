#pragma once

UENUM(BlueprintType)
enum class ETheOneGridSlotType : uint8
{
	None = 0,
	PlayerPropBag = 1,
	PlayerWeaponBag = 2,
	CharacterBag = 3,
	CharacterWeapon = 4,
	ShopSell = 5,
	ShopTreasure = 6
};

