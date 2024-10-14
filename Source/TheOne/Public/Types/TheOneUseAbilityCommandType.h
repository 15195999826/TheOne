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
	EndTurn,
	WaitTurn
};

USTRUCT(BlueprintType)
struct FTheOneUseAbilityCommandPayload
{
	GENERATED_BODY()

	FTheOneUseAbilityCommandPayload()
		: CommandType(), IntPayload(0)
		  , VectorPayload(FVector::ZeroVector), FloatPayload(0), ActorPayload(nullptr)
	{
	}

	FTheOneUseAbilityCommandPayload(ETheOneUseAbilityCommandType InCommandType, int32 InIntPayload, const FVector& InVectorPayload, float InFloatPayload)
		: CommandType(InCommandType), IntPayload(InIntPayload)
		  , VectorPayload(InVectorPayload), FloatPayload(InFloatPayload), ActorPayload(nullptr)
	{
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETheOneUseAbilityCommandType CommandType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 IntPayload;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector VectorPayload;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FloatPayload;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> ActorPayload;
};
