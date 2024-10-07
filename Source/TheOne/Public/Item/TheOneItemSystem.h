// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TheOneCharacterUnique.h"
#include "Types/TheOneGridSlotType.h"
#include "Types/TheOneItem.h"
#include "TheOneItemSystem.generated.h"

USTRUCT(BlueprintType)
struct FTheOneItemInstance
{
	GENERATED_BODY()

	FTheOneItemInstance(): ItemID(0), ItemType(), LogicSlotID(INDEX_NONE)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	int32 ItemID;

	UPROPERTY(BlueprintReadOnly)
	FName ItemRowName;

	UPROPERTY(BlueprintReadOnly)
	ETheOneItemType ItemType;
	
	UPROPERTY(BlueprintReadOnly)
	int32 LogicSlotID;
	
	// Todo: 其它可养成数据
};


USTRUCT()
struct FTheOneLogicSlotInfo
{
	GENERATED_BODY()

	FTheOneLogicSlotInfo(): SlotType(ETheOneGridSlotType::None), OwnerFlag(0)
	{
	}
	
	ETheOneGridSlotType SlotType;

	// 对于CharacterWeapon、CharacterBag， 该值为CharacterAICtrl的UniqueID
	uint32 OwnerFlag;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTheOneItemChangedDelegate, const FTheOneItemInstance&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTheOneItemUpdateDelegate, int32 ,const FTheOneItemInstance&);

/**
 * 负责管理逻辑层道具的生成，删除，移动
 * 道具功能的实现放在TheOneEquipItemComponent
 * UI上的渲染放在TheOneUIRoot与TheOneCharacterPanel
 */
UCLASS()
class THEONE_API UTheOneItemSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	FOnTheOneItemChangedDelegate OnItemCreated;
	FOnTheOneItemChangedDelegate OnItemDeleted;
	FOnTheOneItemUpdateDelegate OnItemUpdated;
	FOnTheOneItemUpdateDelegate OnPostItemUpdated;

protected:
	// <ItemID, ItemInstance>
	TMap<int32, FTheOneItemInstance> ItemInstanceMap;
	// <SlotID, SlotInfo>
	TMap<int32, FTheOneLogicSlotInfo> LogicSlotMap;

	// 方便快速查询
	TMap<uint32, int32> CharacterMainHandSlotMap;
	TMap<uint32, int32> CharacterOffHandSlotMap;
	TMap<uint32, int32> CharacterHeadSlotMap;
	TMap<uint32, int32> CharacterLeftJewelrySlotMap;
	TMap<uint32, int32> CharacterRightJewelrySlotMap;
	TMap<uint32, int32> CharacterClothSlotMap;
	TMap<uint32, TArray<int32>> CharacterStoreSlotMap;
	TArray<int32> ShopTreasureSlotIDs;
	TArray<int32> PlayerTeamSlotIDs;
	TArray<int32> PlayerBagSlotIDs;
public:
	int32 CreateItemInstance(const FName& InItemRowName, ETheOneItemType InItemType, int32 InLogicSlotID = INDEX_NONE, TFunction<void(int)> DeferredFunc = nullptr);
	void DeleteItem(int32 InItemID);
	void DeleteItemByLogicSlotID(int32 InLogicSlotID);
	void DeleteItemByLogicSlotID(TArray<int32> InLogicSlotIDs);
	void MoveItem(int32 InItemID, int32 ToSlotID);
	void RegisterCharacterSlots(uint32 CharacterAICtrlID);
	int32 RegisterOnePlayerSlot(ETheOneGridSlotType InSlotType);

	const TArray<int32>& GetShopTreasureSlotIDs() const;
	const TArray<int32>& GetPlayerTeamSlotIDs() const;

	int32 FindCharacterItemSlotID(uint32 CharacterAICtrlID, ETheOneCharacterBagSlotType InType, int InIndex = 0);
	ETheOneCharacterBagSlotType SlotID2CharacterBagSlotType(uint32 CharacterAICtrlID, int32 InSlotID) const;

	const FTheOneLogicSlotInfo* GetLogicSlotInfo(int32 InSlotID) const;
	
	int32 FindFirstEmptyTeamSlotID();
	int32 FindFirstEmptyPlayerBagSlotID();
	
private:
	int32 IntervalCreateEquipmentInstance(const FName& InWeaponRowName, const FTheOneEquipmentConfig* InWeaponConfig, int32 InSlotID, TFunction<void(int)> DeferredFunc);
	int32 IntervalCreateMinionInstance(const FName& InCharacterConfigRowName, int32 InSlotID, TFunction<void(int)> DeferredFunc);

	bool HasItemOnSlot(int SlotID);
public:
	const FTheOneItemInstance* FindItem(int32 InItemID) const;
	
private:
	int32 NextItemID = 0;
	int32 NextLogicSlotID = 0;
	
	int32 GetNextSlotID()
	{
		return NextLogicSlotID++;
	}
};
