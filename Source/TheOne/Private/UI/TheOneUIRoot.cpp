// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TheOneUIRoot.h"

#include "TheOneLogChannels.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/OverlaySlot.h"
#include "Development/TheOneDataTableSettings.h"
#include "Item/TheOneItemSystem.h"
#include "Subsystems/TheOneContextSystem.h"

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

void UTheOneUIRoot::RegisterGridSlot(int ID, UTheOneGridSlot* InGridSlot)
{
	// Todo: 商店珍藏也需要注册不可见的Slot
	if (GridSlotMap.Contains(ID))
	{
		// 打印错误日志
		auto Parent = InGridSlot->GetParent();
		auto ParentName = Parent ? Parent->GetName() : TEXT("Unknown");
		UE_LOG(LogTheOne, Error, TEXT("[%s].[%s], 注册ID%d失败"), *GetName(), *ParentName, ID);
		return;
	}
	
	GridSlotMap.Add(ID, InGridSlot);
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	switch (InGridSlot->OwnerType) {
		case ETheOneGridSlotType::None:
			UE_LOG(LogTheOne, Error, TEXT("TheOneGridSlot[%s] OwnerType is None"), *InGridSlot->GetName());
			break;
		case ETheOneGridSlotType::CharacterBag:
		case ETheOneGridSlotType::CharacterWeapon:
		case ETheOneGridSlotType::ShopSell:
			break;
		case ETheOneGridSlotType::PlayerPropBag:
		case ETheOneGridSlotType::PlayerWeaponBag:
		case ETheOneGridSlotType::ShopTreasure:
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
		case ETheOneGridSlotType::PlayerWeaponBag:
			ToSlotID = UISlotToLogicSlotMap[InUISlotID];
			break;
		case ETheOneGridSlotType::CharacterBag:
			{
				auto SelectedCharacter = GetWorld()->GetSubsystem<UTheOneContextSystem>()->WeakSelectedCharacter;
				check(SelectedCharacter.IsValid());
				auto Index = InUISlotID % 6;
				ToSlotID = ItemSystem->FindCharacterBagSlotID(SelectedCharacter->GetFlag(), Index);
			}
			break;
		case ETheOneGridSlotType::CharacterWeapon:
			{
				auto SelectedCharacter = GetWorld()->GetSubsystem<UTheOneContextSystem>()->WeakSelectedCharacter;
				check(SelectedCharacter.IsValid());
				ToSlotID = ItemSystem->FindWeaponSlotID(SelectedCharacter->GetFlag());
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

void UTheOneUIRoot::OnItemCreated(const FTheOneItemInstance& InItemInstance)
{
	auto ItemWidget = GetItemWidget();
	if (InItemInstance.ItemType == ETheOneItemType::Weapon)
	{
		auto DT = GetDefault<UTheOneDataTableSettings>()->WeaponTable.LoadSynchronous();
		auto WeaponRow = DT->FindRow<FTheOneWeaponConfig>(InItemInstance.ItemRowName, "UTheOneUIRoot::OnItemCreated");
		check(WeaponRow);
		ItemWidget->SetupWeaponConfig(false, InItemInstance.ItemRowName, *WeaponRow, InItemInstance.ItemID);
	}
	else
	{
		// Todo:
	}
	check(ItemWidgetMap.Contains(InItemInstance.ItemID) == false);
	ItemWidgetMap.Add(InItemInstance.ItemID, ItemWidget);
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
		case ETheOneGridSlotType::PlayerWeaponBag:
		case ETheOneGridSlotType::PlayerPropBag:
			{
				auto UISlotID = LogicSlotToUISlotMap[InItemInstance.LogicSlotID];
				UE_LOG(LogTheOne, Log, TEXT("Item[%d] Update to UISlot[%d]"), InItemInstance.ItemID, UISlotID);
				auto GridSlot = GridSlotMap[UISlotID];
				GridSlot->SetContent(ItemWidget);
			}
			break;
		case ETheOneGridSlotType::CharacterWeapon:
			{
				
			}
			break;
		case ETheOneGridSlotType::CharacterBag:
			break;
		case ETheOneGridSlotType::ShopSell:
			break;
	}

}
