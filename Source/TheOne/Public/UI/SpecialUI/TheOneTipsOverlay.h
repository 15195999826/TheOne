// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/TheOneFocusData.h"
#include "TheOneTipsOverlay.generated.h"

struct FHTileEnvironment;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneTipsOverlay : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PostGetNewFocus();

	UFUNCTION(BlueprintImplementableEvent)
	void PostFocusTile(const FText& EnvName, int Cost, int Energy, bool bIsBlock);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoseFocus();
private:
	void OnGetFocus(const FTheOneFocusData& TheOneFocusData);

	
};
