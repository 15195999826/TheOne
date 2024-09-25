// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheOneUserWidget.generated.h"

class UTheOneViewController;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTheOneViewController> ViewController;

	UFUNCTION(BlueprintImplementableEvent)
	void ViewControllerProvided(UTheOneViewController* VC);
	
	UFUNCTION(BlueprintCallable)
	void SetViewController(UTheOneViewController* VC);
	
	UFUNCTION(CallInEditor)
	void BuildGrid();
};
