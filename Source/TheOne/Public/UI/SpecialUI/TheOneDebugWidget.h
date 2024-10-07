// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheOneDebugWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneDebugWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void AddMinion(const FName& MinionRowName);
	
	UFUNCTION(BlueprintCallable)
	void AddEquipment(const FName& EquipmentRowName);
};
