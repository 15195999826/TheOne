// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AXELandMain.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UAXELandMain : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = Input,
		meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
	TObjectPtr<UUserWidget> InputActionWidget;
};
