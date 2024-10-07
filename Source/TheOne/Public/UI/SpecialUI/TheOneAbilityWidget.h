// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TheOneShortcutInterface.h"
#include "TheOneAbilityWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAbilityWidget : public UUserWidget, public ITheOneShortcutInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CDCover;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CDText;
	
	void BindAbilityData(UTheOneGeneralGA* GA);

	void UnBindAbilityData();
};
