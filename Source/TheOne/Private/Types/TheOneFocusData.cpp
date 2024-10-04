#include "Types/TheOneFocusData.h"

#include "Character/TheOneCharacterBase.h"

FString FTheOneFocusData::ToString() const
{
	static const TMap<ETheOneFocusType, FString> FocusTypeStringMap = {
		{ETheOneFocusType::None, TEXT("FocusNone")},
		{ETheOneFocusType::Character, TEXT("FocusCharacter")},
		{ETheOneFocusType::Tile, TEXT("FocusTile")},
	};
	return FString::Printf(TEXT("FocusType: %s, TileIndex: %d, FocusCharacter: %s"), *FocusTypeStringMap[FocusType], TileIndex, FocusCharacter.IsValid()?*FocusCharacter->GetName():TEXT("None"));
}
