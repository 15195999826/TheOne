#pragma once

UENUM(BlueprintType)
enum class ETheOneGridSlotType : uint8
{
	None = 0,
	PlayerPropBag = 1,
	CharacterBag = 3,
	ShopSell = 5,
	ShopTreasure = 6,
	PlayerTeamBag = 7,
};

UENUM(BlueprintType)
enum class ETheOneCharacterBagSlotType : uint8
{
	MainHand = 0, // 主手只能放武器
	OffHand = 1, // 副手只能放盾牌或者消耗品？
	Head = 2,
	Cloth = 3,
	LeftJewelry = 4,
	RightJewelry = 5,
	Store = 6,
};

