// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/TheOneItem.h"
#include "TheOneItemUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneItemUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsPreview;
	
	UPROPERTY(BlueprintReadOnly)
	FName RowName;
	
	UPROPERTY(BlueprintReadOnly)
	int ItemID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Draggable = true;

	UPROPERTY(BlueprintReadOnly)
	ETheOneItemType ItemType;

public:
	UFUNCTION(BlueprintNativeEvent)
	void Clear();

	UFUNCTION(BlueprintNativeEvent)
	void SetupWeaponConfig(bool IsPreview, const FName& InRowName, const FTheOneWeaponConfig& WeaponConfig, int InItemID = -1);
};
