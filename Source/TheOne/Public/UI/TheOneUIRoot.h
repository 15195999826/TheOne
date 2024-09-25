// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Item/TheOneItemSystem.h"
#include "SpecialUI/TheOneItemUserWidget.h"
#include "Types/TheOneGridSlotType.h"
#include "TheOneUIRoot.generated.h"

class UTheOneGridSlot;

UENUM(BlueprintType)
enum class ETheOneUIOverlayType : uint8
{
	BackEnd,
	Main,
	FrontEnd
};


/**
 * 
 */
UCLASS()
class THEONE_API UTheOneUIRoot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UTheOneItemUserWidget> ItemWidgetClass;

protected:
	// Overlay, 自下而上， BackEnd , Main, FrontEnd
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> BackEndOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> MainOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> FrontEndOverlay;
	
	TMap<int32, TObjectPtr<UTheOneGridSlot>> GridSlotMap;

	TMap<int32, int32> LogicSlotToUISlotMap;
	TMap<int32, int32> UISlotToLogicSlotMap;

	TQueue<TObjectPtr<UTheOneItemUserWidget>> ItemWidgetPool;

	UPROPERTY()
	TMap<int32, TObjectPtr<UTheOneItemUserWidget>> ItemWidgetMap;

public:
	void PushUI(ETheOneUIOverlayType OverlayType, UUserWidget* Widget, TEnumAsByte<EHorizontalAlignment> HorizontalAlignment, TEnumAsByte<EVerticalAlignment> VerticalAlignment) const;

	void RegisterGridSlot(int32 ID, UTheOneGridSlot* InGridSlot);

	UTheOneItemUserWidget* FindItemWidget(int32 InItemID);
	void DropItemOnGridSlot(int32 InUISlotID, ETheOneGridSlotType InUISlotType, UTheOneItemUserWidget* ItemVC);

private:
	UTheOneItemUserWidget* GetItemWidget();
	
	void RecycleItemWidget(UTheOneItemUserWidget* InItemWidget);
	
	void OnItemCreated(const FTheOneItemInstance& InItemInstance);
	void OnItemDeleted(const FTheOneItemInstance& InItemInstance);
	void OnItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance);
};
