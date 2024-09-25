// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TheOneGameplayTags.h"
#include "Engine/DeveloperSettings.h"
#include "TheOneStructDeveloperSettings.generated.h"
/**
 * 
 */
UCLASS(config=Game, defaultconfig)
class THEONE_API UTheOneStructDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// inline static FString 
	
	UFUNCTION()
	static  TArray<FString> GetCustomKVsValues()
	{
		return { TheOneGameplayTags::Debug_GE.GetTag().ToString() };
	}
};
