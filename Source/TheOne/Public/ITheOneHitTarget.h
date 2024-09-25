// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITheOneHitTarget.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTheOneHitTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEONE_API ITheOneHitTarget
{
	GENERATED_IINTERFACE_BODY()

	virtual AActor* GetHitTarget() const = 0;
};
