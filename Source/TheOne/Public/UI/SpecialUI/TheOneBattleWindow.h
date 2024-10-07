// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/TheOneCharacterBase.h"
#include "Interface/TheOneActivableWindowInterface.h"
#include "TheOneBattleWindow.generated.h"

class UTheOneAbilityPanel;
class UTheOneCharacterPanel;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneBattleWindow : public UUserWidget, public ITheOneActivableWindowInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// ITheOneActivableWindowInterface Start
public:
	virtual void OnActive_Implementation() override;
	virtual void OnDeActive_Implementation() override;
	
	// ITheOneActivableWindowInterface End
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneCharacterPanel> CharacterPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAbilityPanel> AbilityPanel;

private:
	void OnCharacterGetTurn(ATheOneCharacterBase* InCharacter);
	void OnCharacterEndTurn(ATheOneCharacterBase* InCharacter);
	
};
