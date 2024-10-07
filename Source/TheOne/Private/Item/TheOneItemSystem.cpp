// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TheOneItemSystem.h"

#include "TheOneLogChannels.h"
#include "AI/TheOneAIController.h"
#include "Development/TheOneDataTableSettings.h"
#include "Game/TheOneGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UTheOneItemSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

// Todo: 替换TFunction为Delegate可以降低开销
int32 UTheOneItemSystem::CreateItemInstance(const FName& InItemRowName, ETheOneItemType InItemType, int32 InLogicSlotID,
	TFunction<void(int)> DeferredFunc)
{
	UE_LOG(LogTheOne, Log, TEXT("CreateItemInstance %s, At Slot %d"), *InItemRowName.ToString(), InLogicSlotID);
	UDataTable* DT;
	switch (InItemType)
	{
		case ETheOneItemType::Equipment:
			{
				DT = GetDefault<UTheOneDataTableSettings>()->EquipmentTable.LoadSynchronous();
				auto WeaponRow = DT->FindRow<FTheOneEquipmentConfig>(InItemRowName,
																"UTheOneItemWorldSubsystem::CreateEquipmentInstance By FName");
				return IntervalCreateEquipmentInstance(InItemRowName, WeaponRow, InLogicSlotID, DeferredFunc);
			}
		case ETheOneItemType::Minion:
			{
				return IntervalCreateMinionInstance(InItemRowName, InLogicSlotID, DeferredFunc);
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
	// Todo: 现在总是注册4个的, 但是这里可以变动， 可以获得更多的存储格子
	TArray<int32> BagSlotIDs;
	for (int32 i = 0; i < 4; i++)
	{
		FTheOneLogicSlotInfo SlotInfo;
		SlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
		SlotInfo.OwnerFlag = CharacterAICtrlID;
		auto ID = GetNextSlotID();
		LogicSlotMap.Add(ID, SlotInfo);
		BagSlotIDs.Add(ID);
	}
	CharacterStoreSlotMap.Add(CharacterAICtrlID, BagSlotIDs);

	FTheOneLogicSlotInfo LeftWeaponSlotInfo;
	LeftWeaponSlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	LeftWeaponSlotInfo.OwnerFlag = CharacterAICtrlID;
	auto LeftWeaponSlotID = GetNextSlotID();
	LogicSlotMap.Add(LeftWeaponSlotID, LeftWeaponSlotInfo);
	CharacterMainHandSlotMap.Add(CharacterAICtrlID, LeftWeaponSlotID);

	FTheOneLogicSlotInfo RightWeaponSlotInfo;
	RightWeaponSlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	RightWeaponSlotInfo.OwnerFlag = CharacterAICtrlID;
	auto RightWeaponSlotID = GetNextSlotID();
	LogicSlotMap.Add(RightWeaponSlotID, RightWeaponSlotInfo);
	CharacterOffHandSlotMap.Add(CharacterAICtrlID, RightWeaponSlotID);

	FTheOneLogicSlotInfo HeadSlotInfo;
	HeadSlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	HeadSlotInfo.OwnerFlag = CharacterAICtrlID;
	auto HeadSlotID = GetNextSlotID();
	LogicSlotMap.Add(HeadSlotID, HeadSlotInfo);
	CharacterHeadSlotMap.Add(CharacterAICtrlID, HeadSlotID);

	FTheOneLogicSlotInfo ClothSlotInfo;
	ClothSlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	ClothSlotInfo.OwnerFlag = CharacterAICtrlID;
	auto ClothSlotID = GetNextSlotID();
	LogicSlotMap.Add(ClothSlotID, ClothSlotInfo);
	CharacterClothSlotMap.Add(CharacterAICtrlID, ClothSlotID);

	FTheOneLogicSlotInfo LeftJewelrySlotInfo;
	LeftJewelrySlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	LeftJewelrySlotInfo.OwnerFlag = CharacterAICtrlID;
	auto LeftJewelrySlotID = GetNextSlotID();
	LogicSlotMap.Add(LeftJewelrySlotID, LeftJewelrySlotInfo);
	CharacterLeftJewelrySlotMap.Add(CharacterAICtrlID, LeftJewelrySlotID);

	FTheOneLogicSlotInfo RightJewelrySlotInfo;
	RightJewelrySlotInfo.SlotType = ETheOneGridSlotType::CharacterBag;
	RightJewelrySlotInfo.OwnerFlag = CharacterAICtrlID;
	auto RightJewelrySlotID = GetNextSlotID();
	LogicSlotMap.Add(RightJewelrySlotID, RightJewelrySlotInfo);
	CharacterRightJewelrySlotMap.Add(CharacterAICtrlID, RightJewelrySlotID);
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
	else if (InSlotType == ETheOneGridSlotType::PlayerTeamBag)
	{
		PlayerTeamSlotIDs.Add(ID);
	}
	else if (InSlotType == ETheOneGridSlotType::PlayerPropBag)
	{
		PlayerBagSlotIDs.Add(ID);
	}
	
	return ID;
}

const TArray<int32>& UTheOneItemSystem::GetShopTreasureSlotIDs() const
{
	return ShopTreasureSlotIDs;
}

const TArray<int32>& UTheOneItemSystem::GetPlayerTeamSlotIDs() const
{
	return PlayerTeamSlotIDs;
}

int32 UTheOneItemSystem::FindCharacterItemSlotID(uint32 CharacterAICtrlID, ETheOneCharacterBagSlotType InType,
                                                 int InIndex)
{
	switch (InType) {
		case ETheOneCharacterBagSlotType::MainHand:
			return CharacterMainHandSlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::OffHand:
			return CharacterOffHandSlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::Head:
			return CharacterHeadSlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::Cloth:
			return CharacterClothSlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::LeftJewelry:
			return CharacterLeftJewelrySlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::RightJewelry:
			return CharacterRightJewelrySlotMap[CharacterAICtrlID];
		case ETheOneCharacterBagSlotType::Store:
			return CharacterStoreSlotMap[CharacterAICtrlID][InIndex];
	}

	return INDEX_NONE;
}

ETheOneCharacterBagSlotType UTheOneItemSystem::SlotID2CharacterBagSlotType(uint32 CharacterAICtrlID,
	int32 InSlotID) const
{
	if (CharacterMainHandSlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::MainHand;
	}

	if (CharacterOffHandSlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::OffHand;
	}

	if (CharacterHeadSlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::Head;
	}

	if (CharacterClothSlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::Cloth;
	}

	if (CharacterLeftJewelrySlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::LeftJewelry;
	}

	if (CharacterRightJewelrySlotMap[CharacterAICtrlID] == InSlotID)
	{
		return ETheOneCharacterBagSlotType::RightJewelry;
	}

	return ETheOneCharacterBagSlotType::Store;
}

const FTheOneLogicSlotInfo* UTheOneItemSystem::GetLogicSlotInfo(int32 InSlotID) const
{
    return LogicSlotMap.Find(InSlotID);
}


int32 UTheOneItemSystem::FindFirstEmptyTeamSlotID()
{
	for (auto SlotID : PlayerTeamSlotIDs)
	{
		if (!HasItemOnSlot(SlotID))
		{
			return SlotID;
		}
	}
	return INDEX_NONE;
}

int32 UTheOneItemSystem::FindFirstEmptyPlayerBagSlotID()
{
	for (auto SlotID : PlayerBagSlotIDs)
	{
		if (!HasItemOnSlot(SlotID))
		{
			return SlotID;
		}
	}
	return INDEX_NONE;
}

int32 UTheOneItemSystem::IntervalCreateEquipmentInstance(const FName& InWeaponRowName,
                                                         const FTheOneEquipmentConfig* InWeaponConfig, int32 InSlotID, TFunction<void(int)> DeferredFunc)
{
	NextItemID++;
	FTheOneItemInstance WeaponInstance;
	WeaponInstance.ItemID = NextItemID;
	WeaponInstance.ItemRowName = InWeaponRowName;
	WeaponInstance.ItemType = ETheOneItemType::Equipment;
	WeaponInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, WeaponInstance);
	if (DeferredFunc)
	{
		DeferredFunc(NextItemID);
	}
	OnItemCreated.Broadcast(WeaponInstance);
	return NextItemID;
}

int32 UTheOneItemSystem::IntervalCreateMinionInstance(const FName& InCharacterConfigRowName, int32 InSlotID, TFunction<void(int)> DeferredFunc)
{
	NextItemID++;
	FTheOneItemInstance PropInstance;
	PropInstance.ItemID = NextItemID;
	PropInstance.ItemRowName = InCharacterConfigRowName;
	PropInstance.ItemType = ETheOneItemType::Minion;
	PropInstance.LogicSlotID = InSlotID;
	ItemInstanceMap.Add(NextItemID, PropInstance);
	if (DeferredFunc)
	{
		DeferredFunc(NextItemID);
	}
	OnItemCreated.Broadcast(PropInstance);
	return NextItemID;
}

bool UTheOneItemSystem::HasItemOnSlot(int SlotID)
{
	for (auto& ItemPair : ItemInstanceMap)
	{
		if (ItemPair.Value.LogicSlotID == SlotID)
		{
			return true;
		}
	}

	return false;
}

const FTheOneItemInstance* UTheOneItemSystem::FindItem(int32 InItemID) const
{
	return ItemInstanceMap.Find(InItemID);
}
