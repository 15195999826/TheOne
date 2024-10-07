#pragma once

#include "CoreMinimal.h"
#include "TheOneUseAbilityCommandType.generated.h"

UENUM(BlueprintType)
enum class ETheOneUseAbilityCommandType : uint8
{
	Invalid,
	UseItem,
	UseWeaponAbility,
	Move,
};

USTRUCT(BlueprintType)
struct FTheOneUseAbilityCommandPayload
{
	GENERATED_BODY()

	FTheOneUseAbilityCommandPayload()
		: CommandType(), IntPayload(0)
		  , VectorPayload(FVector::ZeroVector)
	{
	}

	FTheOneUseAbilityCommandPayload(ETheOneUseAbilityCommandType InCommandType, int32 InIntPayload, const FVector& InVectorPayload)
		: CommandType(InCommandType), IntPayload(InIntPayload)
		  , VectorPayload(InVectorPayload)
	{
	}

	UPROPERTY(BlueprintReadWrite)
	ETheOneUseAbilityCommandType CommandType;

	UPROPERTY(BlueprintReadWrite)
	int32 IntPayload;

	UPROPERTY(BlueprintReadWrite)
	FVector VectorPayload;
};
