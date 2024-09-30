#pragma once

#include "CoreMinimal.h"
#include "HexGrid/HGTypes.h"
#include "TheOneFocusData.generated.h"

UENUM(BlueprintType)
enum class ETheOneFocusType : uint8
{
	None,
	Character,
	Tile,
};

USTRUCT(BlueprintType)
struct FTheOneFocusData
{

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETheOneFocusType FocusType = ETheOneFocusType::None;

	UPROPERTY(BlueprintReadOnly)
	int TileIndex;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class ATheOneCharacterBase> FocusCharacter;
};
