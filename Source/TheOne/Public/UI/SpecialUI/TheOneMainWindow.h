// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheOneMainWindow.generated.h"

class ATheOneCharacterBase;
class UTheOneShop;
class UTheOnePlayerBag;
class UTheOneCharacterPanel;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneMainWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneCharacterPanel> CharacterPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOnePlayerBag> PlayerBag;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneShop> Shop;

public:
	UFUNCTION(BlueprintCallable)
	void InitMainWindow();

protected:
	void OnSelectCharacter(ATheOneCharacterBase* InTheOneCharacterBase);
	void OnDeSelectCharacter();
};
