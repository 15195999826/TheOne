// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITheOneCharacterSpawnActor.generated.h"

class ATheOneCharacterBase;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTheOneCharacterSpawnActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEONE_API ITheOneCharacterSpawnActor
{
	GENERATED_IINTERFACE_BODY()

	virtual ATheOneCharacterBase* GetSourceCharacter() const = 0;
};
