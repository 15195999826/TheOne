// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneShop.h"

#include "Components/UniformGridPanel.h"
#include "Development/TheOneDataTableSettings.h"
#include "Game/TheOneGameInstance.h"
#include "Item/TheOneItemSystem.h"
#include "Types/TheOneItem.h"
#include "UI/TheOneUIRoot.h"

void UTheOneShop::Init()
{
	
}

void UTheOneShop::RefreshTreasure()
{
	// auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	// // 临时： 刷新出全部的武器， 创建武器实例
	// auto WeaponDT = GetDefault<UTheOneDataTableSettings>()->EquipmentTable.LoadSynchronous();
	// auto RowMap = WeaponDT->GetRowMap();
	// auto Count = 0;
	// const auto& TreasureLogicSlots = ItemSystem->GetShopTreasureSlotIDs();
	// ItemSystem->DeleteItemByLogicSlotID(TreasureLogicSlots);
	// for (const auto& Elem : RowMap)
	// {
	// 	if (Count >= TreasureLogicSlots.Num())
	// 	{
	// 		break;
	// 	}
	// 	
	// 	ItemSystem->CreateItemInstance(Elem.Key, ETheOneItemType::Equipment, TreasureLogicSlots[Count]);
	// 	Count++;
	// }
}
