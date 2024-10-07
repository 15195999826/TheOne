// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Item/TheOneItemSystem.h"
#include "SpecialUI/TheOneItemUserWidget.h"
#include "Types/TheOneGridSlotType.h"
#include "Types/TheOneImportantUI.h"
#include "TheOneUIRoot.generated.h"

class UTheOneBattleWindow;
class UTheOneMainWindow;
class UTheOneGridSlot;

UENUM(BlueprintType)
enum class ETheOneUIOverlayType : uint8
{
	BackEnd,
	Main,
	FrontEnd
};

/**
 * Window只进行隐藏和现实， Window内部的小部件，如果使用了池化技术， 就需要注意其几个构造函数了
 * 对于实现了PreConstruct\Initialized\Construct的小部件， 如果进行了持久化存储，一定要注意其在AddToVP和RemoveFromParent时会反复调用
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

	UPROPERTY(BlueprintReadOnly)
	UTheOneMainWindow* MainWindow;
	UPROPERTY(BlueprintReadOnly)
	UTheOneBattleWindow* BattleWindow;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	TSubclassOf<UTheOneMainWindow> MainWindowClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	TSubclassOf<UTheOneBattleWindow> BattleWindowClass;
	
	// Overlay, 自下而上， BackEnd , Main, FrontEnd
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> BackEndOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> MainOverlay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> FrontEndOverlay;

	// 将不显示的Widget放在这里
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> HiddenOverlay;
	
	TMap<int32, TObjectPtr<UTheOneGridSlot>> GridSlotMap;

	TMap<int32, int32> LogicSlotToUISlotMap;
	TMap<int32, int32> UISlotToLogicSlotMap;

	TQueue<TObjectPtr<UTheOneItemUserWidget>> ItemWidgetPool;

	UPROPERTY()
	TMap<int32, TObjectPtr<UTheOneItemUserWidget>> ItemWidgetMap;

	

public:
	void PushUI(ETheOneUIOverlayType OverlayType, UUserWidget* Widget, TEnumAsByte<EHorizontalAlignment> HorizontalAlignment, TEnumAsByte<EVerticalAlignment> VerticalAlignment) const;

	void ShowImportantUI(ETheOneImportantUI InUI);
	void CloseImportantUI(ETheOneImportantUI InUI);
	
	void RegisterGridSlot(int32 ID, UTheOneGridSlot* InGridSlot);

	UTheOneItemUserWidget* FindItemWidget(int32 InItemID);
	void DropItemOnGridSlot(int32 InUISlotID, ETheOneGridSlotType InUISlotType, UTheOneItemUserWidget* ItemVC);

	void HideItemWidget(UTheOneItemUserWidget* InWidget) const;
private:
	UTheOneItemUserWidget* GetItemWidget();
	
	void RecycleItemWidget(UTheOneItemUserWidget* InItemWidget);
	
	void OnItemCreated(const FTheOneItemInstance& InItemInstance);
	void OnItemDeleted(const FTheOneItemInstance& InItemInstance);
	void OnItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance);
};
