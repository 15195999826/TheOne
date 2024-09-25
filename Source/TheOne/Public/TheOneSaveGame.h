// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "Character/TheOneCharacterBase.h"
#include "GameFramework/SaveGame.h"
#include "TheOneSaveGame.generated.h"

USTRUCT()
struct FTheOneTeamCharacterArchive
{
	GENERATED_BODY()

	TSubclassOf<ATheOneCharacterBase> CharacterClass;

	FTheOneAttributeSetArchive AttributeSetArchive;
};

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void SaveGame();
};
