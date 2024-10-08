#pragma once

#include "CoreMinimal.h"
#include "TheOneUseAbilityCommandType.generated.h"

UENUM(BlueprintType)
enum class ETheOneUseAbilityCommandType : uint8
{
	Invalid,
	UseItem,
	UseAbility,
	Move,
};

USTRUCT(BlueprintType)
struct FTheOneUseAbilityCommandPayload
{
	GENERATED_BODY()

	FTheOneUseAbilityCommandPayload()
		: CommandType(), IntPayload(0)
		  , VectorPayload(FVector::ZeroVector), FloatPayload(0)
	{
	}

	FTheOneUseAbilityCommandPayload(ETheOneUseAbilityCommandType InCommandType, int32 InIntPayload, const FVector& InVectorPayload, float InFloatPayload)
		: CommandType(InCommandType), IntPayload(InIntPayload)
		  , VectorPayload(InVectorPayload), FloatPayload(InFloatPayload)
	{
	}

	UPROPERTY(BlueprintReadWrite)
	ETheOneUseAbilityCommandType CommandType;

	UPROPERTY(BlueprintReadWrite)
	int32 IntPayload;

	UPROPERTY(BlueprintReadWrite)
	FVector VectorPayload;

	UPROPERTY(BlueprintReadWrite)
	float FloatPayload;
};
