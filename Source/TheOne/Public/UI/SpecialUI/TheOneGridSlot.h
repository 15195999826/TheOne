// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TheOneShortcutInterface.h"
#include "Types/TheOneGridSlotType.h"
#include "Types/TheOneItem.h"
#include "TheOneGridSlot.generated.h"

class UTheOneItemUserWidget;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneGridSlot : public UUserWidget, public ITheOneShortcutInterface
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
	virtual void NativeOnDragEnter( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
	virtual void NativeOnDragLeave( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UNamedSlot> DropOnSlot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	int32 ID;

	// 表子示自己是什么位置的格
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	ETheOneGridSlotType OwnerType;

	void SetContent(UTheOneItemUserWidget* ItemWidget) const;
	// UFUNCTION(BlueprintImplementableEvent)
	// void NativeCallOnChanged();
	//
	// UFUNCTION(BlueprintImplementableEvent)
	// void NativeCallInit();
	//
	
	//
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	// void DoHighlight();
	//
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	// void DeHighlight();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool CanDropOnSlot(UTheOneItemUserWidget* InItemWidget);
	
	// UFUNCTION(BlueprintCallable, BlueprintPure)
	// bool IsInnerItem(int ItemId);
	
};
