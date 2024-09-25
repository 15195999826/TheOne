// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TheOneItemSystem.h"

#include "TheOneLogChannels.h"
#include "Development/TheOneDataTableSettings.h"

void UTheOneItemSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

int32 UTheOneItemSystem::CreateItemInstance(const FDataTableRowHandle& InItemRow, ETheOneItemType InItemType, int32 InLogicSlotID)
{
	switch (InItemType) {
		case ETheOneItemType::Weapon:
			{	auto WeaponRow = InItemRow.GetRow<FTheOneWeaponConfig>(
				"UTheOneItemWorldSubsystem::CreateWeaponInstance By FDataTableRowHandle");
				return IntervalCreateWeaponInstance(InItemRow.RowName, WeaponRow, InLogicSlotID);
			}
		
		case ETheOneItemType::Prop:
			{
				auto PropRow = InItemRow.GetRow<FTheOnePropConfig>(
				"UTheOneItemWorldSubsystem::CreatePropInstance By FDataTableRowHandle");
				return IntervalCreatePropInstance(InItemRow.RowName, PropRow, InLogicSlotID);
			}
	}

	check(false);
	return INDEX_NONE;
}

int32 UTheOneItemSystem::CreateItemInstance(const FName& InItemRowName, ETheOneItemType InItemType, int32 InLogicSlotID)
{
	UE_LOG(LogTheOne, Log, TEXT("CreateItemInstance %s, At Slot %d"), *InItemRowName.ToString(), InLogicSlotID);
	UDataTable* DT;
	switch (InItemType)
	{
		case ETheOneItemType::Weapon:
			{
				DT = GetDefault<UTheOneDataTableSettings>()->WeaponTable.LoadSynchronous();
				auto WeaponRow = DT->FindRow<FTheOneWeaponConfig>(InItemRowName,
																"UTheOneItemWorldSubsystem::CreateWeaponInstance By FName");
				return IntervalCreateWeaponInstance(InItemRowName, WeaponRow, InLogicSlotID);
			}
		case ETheOneItemType::Prop:
			{
				DT = GetDefault<UTheOneDataTableSettings>()->PropTable.LoadSynchronous();
				auto PropRow = DT->FindRow<FTheOnePropConfig>(InItemRowName,
															"UTheOneItemWorldSubsystem::CreatePropInstance By FName");
				return IntervalCreatePropInstance(InItemRowName, PropRow, InLogicSlotID);
			}
	}

	check(false);
	return INDEX_NONE;
}

void UTheOneItemSystem::DeleteItem(int32 InItemID)
{
}

void UTheOneItemSystem::DeleteItemByLogicSlotID(int32 InLogicSlotID)
{
	for (auto& Pair : ItemInstanceMap)
	{
		if (Pair.Value.LogicSlotID == InLogicSlotID)
		{
			DeleteItem(Pair.Key);
			break;
		}
	}
}

void UTheOneItemSystem::DeleteItemByLogicSlotID(TArray<int32> InLogicSlotIDs)
{
	TArray<uint32> ToDeleteItemIDs;
	for (auto& Pair : ItemInstanceMap)
	{
		if (InLogicSlotIDs.Contains(Pair.Value.LogicSlotID))
		{
			ToDeleteItemIDs.Add(Pair.Key);
		}
	}

	for (auto ID : ToDeleteItemIDs)
	{
		DeleteItem(ID);
	}
}

void UTheOneItemSystem::MoveItem(int32 InItemID, int32 ToSlotID)
{
	// 检查Slot上有没有Item
	bool HasItemInSlot = false;
	 for (auto& Pair : ItemInstanceMap)
	 {
		 if (Pair.Value.LogicSlotID == ToSlotID)
		 {
		 	HasItemInSlot = true;
		 	break;
		 }
	 }

	if (HasItemInSlot)
	{
		// Todo: 检查是可以交换
		UE_LOG(LogTheOne, Log, TEXT("Can't move item to slot %d, there is already an item in it"), ToSlotID);
		return;
	}

	auto& Item = ItemInstanceMap[InItemID];
	auto OldSlotID = Item.LogicSlotID;
	Item.LogicSlotID = ToSlotID;
	OnItemUpdated.Broadcast(OldSlotID, Item);
	OnPostItemUpdated.Broadcast(OldSlotID, Item);
}

void UTheOneItemSystem::RegisterCharacterSlots(uint32 CharacterAICtrlID)
{
	TArray<int32> BagSlotIDs;
	for (int32 i = 0; i < 6; i++)
	{
		FTheOneLogicSlotInfo SlotInfo;
		SlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
		SlotInfo.OwnerFlag = CharacterAICtrlID;
		auto ID = GetNextSlotID();
		LogicSlotMap.Add(ID, SlotInfo);
		BagSlotIDs.Add(ID);
	}
	CharacterBagSlotMap.Add(CharacterAICtrlID, BagSlotIDs);

	FTheOneLogicSlotInfo WeaponSlotInfo;
	WeaponSlotInfo.SlotType = ETheOneGridSlotType::CharacterWeapon;
	WeaponSlotInfo.OwnerFlag = CharacterAICtrlID;
	auto WeaponSlotID = GetNextSlotID();
	LogicSlotMap.Add(WeaponSlotID, WeaponSlotInfo);
	CharacterWeaponSlotMap.Add(CharacterAICtrlID, WeaponSlotID);
}

int32 UTheOneItemSystem::RegisterOnePlayerSlot(ETheOneGridSlotType InSlotType)
{
	auto ID = GetNextSlotID();
	FTheOneLogicSlotInfo SlotInfo;
	SlotInfo.SlotType = ETheOneGridSlotType::PlayerPropBag;
	LogicSlotMap.Add(ID, SlotInfo);
	if (InSlotType == ETheOneGridSlotType::ShopTreasure)
	{
		ShopTreasureSlotIDs.Add(ID);
	}
	return ID;
}

const TArray<int32>& UTheOneItemSystem::GetShopTreasureSlotIDs() const
{
	return ShopTreasureSlotIDs;
}

int32 UTheOneItemSystem::FindWeaponSlotID(uint32 CharacterAICtrlID)
{
	return CharacterWeaponSlotMap[CharacterAICtrlID];
}

int32 UTheOneItemSystem::FindCharacterBagSlotID(uint32 CharacterAICtrlID, int InIndex)
{
	return CharacterBagSlotMap[CharacterAICtrlID][InIndex];
}

const FTheOneLogicSlotInfo* UTheOneItemSystem::GetLogicSlotInfo(int32 InSlotID) const
{
    return LogicSlotMap.Find(InSlotID);
}

int32 UTheOneItemSystem::IntervalCreateWeaponInstance(const FName& InWeaponRowName,
                                                    const FTheOneWeaponConfig* InWeaponConfig, int32 InSlotID)
{
	NextItemID++;
	FTheOneItemInstance WeaponInstance;
	WeaponInstance.ItemID = NextItemID;
	WeaponInstance.ItemRowName = InWeaponRowName;
	WeaponInstance.ItemType = ETheOneItemType::Weapon;
	WeaponInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, WeaponInstance);
	OnItemCreated.Broadcast(WeaponInstance);
	return NextItemID;
}

int32 UTheOneItemSystem::IntervalCreatePropInstance(const FName& InPropRowName,
	const FTheOnePropConfig* InPropConfig, int32 InSlotID)
{
	NextItemID++;
	FTheOneItemInstance PropInstance;
	PropInstance.ItemID = NextItemID;
	PropInstance.ItemRowName = InPropRowName;
	PropInstance.ItemType = ETheOneItemType::Prop;
	PropInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, PropInstance);
	OnItemCreated.Broadcast(PropInstance);
	return NextItemID;
}

const FTheOneItemInstance* UTheOneItemSystem::FindItem(int32 InItemID) const
{
	return ItemInstanceMap.Find(InItemID);
}
