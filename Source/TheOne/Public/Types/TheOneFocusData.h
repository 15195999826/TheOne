#pragma once

#include "CoreMinimal.h"
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
	
	FTheOneFocusData(): TileIndex(0)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	ETheOneFocusType FocusType = ETheOneFocusType::None;

	UPROPERTY(BlueprintReadOnly)
	int TileIndex;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class ATheOneCharacterBase> FocusCharacter;

	FString ToString() const;
};
