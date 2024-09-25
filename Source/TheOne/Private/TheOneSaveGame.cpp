// Fill out your copyright notice in the Description page of Project Settings.


#include "TheOneSaveGame.h"

#include "EngineUtils.h"

void UTheOneSaveGame::SaveGame()
{
	auto World = GetWorld();
	// 遍历所有ATheOneCharacterBase
	for (TActorIterator<ATheOneCharacterBase> It(World); It; ++It)
	{
		auto Character = *It;
		FTheOneTeamCharacterArchive Archive;
		Archive.CharacterClass = Character->GetClass();
		Archive.AttributeSetArchive = Character->GetAttributeSet()->SaveRuntimeAttributes();
	}
}
