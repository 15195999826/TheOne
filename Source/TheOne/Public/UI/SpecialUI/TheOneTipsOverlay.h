// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheOneTipsOverlay.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneTipsOverlay : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
};
