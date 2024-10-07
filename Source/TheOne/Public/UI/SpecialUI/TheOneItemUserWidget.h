// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/TheOneItemSystem.h"
#include "Types/TheOneItem.h"
#include "TheOneItemUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneItemUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
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
	void SetupEquipmentConfig(bool IsPreview, const FName& InRowName, const FTheOneEquipmentConfig& EquipmentConfig, int InItemID = -1);

	UFUNCTION(BlueprintNativeEvent)
	void SetupMinionConfig(bool IsPreview, FName MinionTemplateName, int32 InItemID);
};
