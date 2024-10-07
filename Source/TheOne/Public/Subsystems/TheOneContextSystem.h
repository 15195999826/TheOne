// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TheOneContextSystem.generated.h"

class ATheOneCharacterBase;
class AHexGrid;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneContextSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AHexGrid> HexGrid;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakSelectedCharacter;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakBagSelectCharacter;
};
