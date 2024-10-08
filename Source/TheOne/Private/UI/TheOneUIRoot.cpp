// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TheOneUIRoot.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/OverlaySlot.h"
#include "Development/TheOneDataTableSettings.h"
#include "Interface/TheOneActivableWindowInterface.h"
#include "Item/TheOneItemSystem.h"
#include "Subsystems/TheOneContextSystem.h"
#include "UI/SpecialUI/TheOneMainWindow.h"
#include "UI/SpecialUI/TheOneBattleWindow.h"

#include "UI/SpecialUI/TheOneGridSlot.h"

void UTheOneUIRoot::NativeConstruct()
{
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	ItemSystem->OnItemCreated.AddUObject(this, &UTheOneUIRoot::OnItemCreated);
	ItemSystem->OnItemDeleted.AddUObject(this, &UTheOneUIRoot::OnItemDeleted);
	ItemSystem->OnItemUpdated.AddUObject(this, &UTheOneUIRoot::OnItemUpdated);
	Super::NativeConstruct();
}

void UTheOneUIRoot::PushUI(ETheOneUIOverlayType OverlayType, UUserWidget* Widget,
                         TEnumAsByte<EHorizontalAlignment> HorizontalAlignment, TEnumAsByte<EVerticalAlignment> VerticalAlignment) const
{
	// Todo: UIPolicy, 是否单例等
	
	switch (OverlayType)
	{
		case ETheOneUIOverlayType::BackEnd:
			BackEndOverlay->AddChild(Widget);
			break;
		case ETheOneUIOverlayType::Main:
			MainOverlay->AddChild(Widget);
			break;
		case ETheOneUIOverlayType::FrontEnd:
			FrontEndOverlay->AddChild(Widget);
			break;
	}
	auto OverlaySlot = CastChecked<UOverlaySlot>(Widget->Slot);
	OverlaySlot->SetHorizontalAlignment(HorizontalAlignment);
	OverlaySlot->SetVerticalAlignment(VerticalAlignment);
}

void UTheOneUIRoot::ShowImportantUI(ETheOneImportantUI InUI)
{
	switch (InUI) {
		case ETheOneImportantUI::MainWindow:
			{
				if (MainWindow == nullptr)
				{
					MainWindow = CreateWidget<UTheOneMainWindow>(this, MainWindowClass);
					PushUI(ETheOneUIOverlayType::Main, MainWindow, HAlign_Fill, VAlign_Fill);
				}
				MainWindow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ITheOneActivableWindowInterface::Execute_OnActive(MainWindow);
			}
			break;
		case ETheOneImportantUI::BattleWindow:
			{
				if (BattleWindow == nullptr)
				{
					BattleWindow = CreateWidget<UTheOneBattleWindow>(this, BattleWindowClass);
					PushUI(ETheOneUIOverlayType::Main, BattleWindow, HAlign_Fill, VAlign_Fill);
				}
				BattleWindow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ITheOneActivableWindowInterface::Execute_OnActive(BattleWindow);
			}
			break;
	}
}

void UTheOneUIRoot::CloseImportantUI(ETheOneImportantUI InUI)
{
	switch (InUI) {
		case ETheOneImportantUI::MainWindow:
			MainWindow->SetVisibility(ESlateVisibility::Hidden);
			ITheOneActivableWindowInterface::Execute_OnDeActive(MainWindow);
			break;
		case ETheOneImportantUI::BattleWindow:
			BattleWindow->SetVisibility(ESlateVisibility::Hidden);
			ITheOneActivableWindowInterface::Execute_OnDeActive(BattleWindow);
			break;
	}
}

void UTheOneUIRoot::RegisterGridSlot(int ID, UTheOneGridSlot* InGridSlot)
{
	// Todo: 商店珍藏也需要注册不可见的Slot
	if (GridSlotMap.Contains(ID))
	{
		// 打印错误日志
		auto Parent = InGridSlot->GetParent();
		auto ParentName = Parent ? Parent->GetName() : TEXT("Unknown");
		UE_LOG(LogTheOne, Error, TEXT("[%s].[%s], 注册ID%d失败"), *ParentName, *InGridSlot->GetName(), ID);
		return;
	}
	
	GridSlotMap.Add(ID, InGridSlot);
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	switch (InGridSlot->OwnerType) {
		case ETheOneGridSlotType::None:
			UE_LOG(LogTheOne, Error, TEXT("TheOneGridSlot[%s] OwnerType is None"), *InGridSlot->GetName());
			break;
		case ETheOneGridSlotType::CharacterBag:
		case ETheOneGridSlotType::ShopSell:
			break;
		case ETheOneGridSlotType::PlayerPropBag:
		case ETheOneGridSlotType::ShopTreasure:
		case ETheOneGridSlotType::PlayerTeamBag:
			{
				auto LogicID= ItemSystem->RegisterOnePlayerSlot(InGridSlot->OwnerType);
				UISlotToLogicSlotMap.Add(ID, LogicID);
				LogicSlotToUISlotMap.Add(LogicID, ID);
				UE_LOG(LogTheOne, Log, TEXT("Register UISlot[%d] to LogicSlot[%d]"), ID, LogicID);
			}
		break;
	}
}

UTheOneItemUserWidget* UTheOneUIRoot::GetItemWidget()
{
	// 先从ItemWidgetPool中取，如果没有再生成
	if (ItemWidgetPool.IsEmpty())
	{
		auto ItemWidget = CreateWidget<UTheOneItemUserWidget>(this, ItemWidgetClass);
		return ItemWidget;
	}

	TObjectPtr<UTheOneItemUserWidget> ItemWidget;
	ItemWidgetPool.Dequeue(ItemWidget);
	ItemWidget->SetVisibility(ESlateVisibility::Visible);
	return ItemWidget;
}

void UTheOneUIRoot::RecycleItemWidget(UTheOneItemUserWidget* InItemWidget)
{
	InItemWidget->Clear();
	InItemWidget->RemoveFromParent();
	InItemWidget->SetVisibility(ESlateVisibility::Hidden);
	ItemWidgetPool.Enqueue(InItemWidget);
}

UTheOneItemUserWidget* UTheOneUIRoot::FindItemWidget(int32 InItemID)
{
	if (ItemWidgetMap.Contains(InItemID))
	{
		return ItemWidgetMap[InItemID];
	}
	UE_LOG(LogTheOne, Error, TEXT("Can't find ItemWidget with ID: %d"), InItemID);
	return nullptr;
}

void UTheOneUIRoot::DropItemOnGridSlot(int32 InUISlotID, ETheOneGridSlotType InUISlotType, UTheOneItemUserWidget* ItemVC)
{
	int32 ToSlotID = INDEX_NONE;
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	switch (InUISlotType) {
		case ETheOneGridSlotType::None:
			break;
		case ETheOneGridSlotType::PlayerPropBag:
		case ETheOneGridSlotType::PlayerTeamBag:
			ToSlotID = UISlotToLogicSlotMap[InUISlotID];
			break;
		case ETheOneGridSlotType::CharacterBag:
			{
				auto SelectedCharacter = GetWorld()->GetSubsystem<UTheOneContextSystem>()->WeakBagSelectCharacter;
				if (!SelectedCharacter.IsValid())
                {
                    UE_LOG(LogTheOne, Error, TEXT("DropItemOnGridSlot Failed, No Selected Bag Character"));
                    return;
                }
				// 在UI上手动设置SlotID， 是从6W开始的， 0~5对应角色装备, 大于6时，是角色背包， 减去6为背包索引
				ETheOneCharacterBagSlotType InType;
				int InIndex;
				UTheOneBlueprintFunctionLibrary::CharacterBagSlotIDToType(InUISlotID, InType, InIndex);
				
				ToSlotID = ItemSystem->FindCharacterItemSlotID(SelectedCharacter->GetFlag(), InType, InIndex);
			}
			break;
		case ETheOneGridSlotType::ShopSell:
			break;
		case ETheOneGridSlotType::ShopTreasure:
			return;
	}
	check(ToSlotID != INDEX_NONE);
	// auto LogicID =  
	
	if (ItemVC->bIsPreview)
	{
			
	}
	else
	{
		ItemSystem->MoveItem(ItemVC->ItemID, ToSlotID);
	}
}

void UTheOneUIRoot::HideItemWidget(UTheOneItemUserWidget* InWidget) const
{
	InWidget->RemoveFromParent();
	HiddenOverlay->AddChild(InWidget);
}

void UTheOneUIRoot::OnItemCreated(const FTheOneItemInstance& InItemInstance)
{
	auto ItemWidget = GetItemWidget();
	if (InItemInstance.ItemType == ETheOneItemType::Equipment)
	{
		auto DT = GetDefault<UTheOneDataTableSettings>()->EquipmentTable.LoadSynchronous();
		auto EquipmentRow = DT->FindRow<FTheOneEquipmentConfig>(InItemInstance.ItemRowName, "UTheOneUIRoot::OnItemCreated");
		check(EquipmentRow);
		ItemWidget->SetupEquipmentConfig(false, InItemInstance.ItemRowName, *EquipmentRow, InItemInstance.ItemID);
	}
	else if (InItemInstance.ItemType == ETheOneItemType::Minion)
	{
		// Todo:
		ItemWidget->SetupMinionConfig(false, InItemInstance.ItemRowName, InItemInstance.ItemID);
	}
	check(ItemWidgetMap.Contains(InItemInstance.ItemID) == false);
	ItemWidgetMap.Add(InItemInstance.ItemID, ItemWidget);
	HideItemWidget(ItemWidget);
	OnItemUpdated(INDEX_NONE, InItemInstance);
}

void UTheOneUIRoot::OnItemDeleted(const FTheOneItemInstance& InItemInstance)
{
}

void UTheOneUIRoot::OnItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance)
{
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	const auto SlotInfo = ItemSystem->GetLogicSlotInfo(InItemInstance.LogicSlotID);
	auto ItemWidget = FindItemWidget(InItemInstance.ItemID);
	ItemWidget->RemoveFromParent();
	switch (SlotInfo->SlotType) {
		case ETheOneGridSlotType::None:
			break;
		case ETheOneGridSlotType::ShopTreasure:
		case ETheOneGridSlotType::PlayerPropBag:
		case ETheOneGridSlotType::PlayerTeamBag:
			{
				auto UISlotID = LogicSlotToUISlotMap[InItemInstance.LogicSlotID];
				UE_LOG(LogTheOne, Log, TEXT("Item[%d] Update to UISlot[%d]"), InItemInstance.ItemID, UISlotID);
				auto GridSlot = GridSlotMap[UISlotID];
				GridSlot->SetContent(ItemWidget);
			}
			break;
		case ETheOneGridSlotType::CharacterBag:
			break;
		case ETheOneGridSlotType::ShopSell:
			break;
	}

}
