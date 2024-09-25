// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TheOneGridSlotType.h"
#include "Types/TheOneItem.h"
#include "TheOneItemSystem.generated.h"

USTRUCT(BlueprintType)
struct FTheOneItemInstance
{
	GENERATED_BODY()

	FTheOneItemInstance(): ItemID(0), ItemType(), LogicSlotID(0)
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
	TMap<int32, FTheOneItemInstance> ItemInstanceMap;
	TMap<int32, FTheOneLogicSlotInfo> LogicSlotMap;

	// 方便快速查询
	TMap<uint32, int32> CharacterWeaponSlotMap;
	TMap<uint32, TArray<int32>> CharacterBagSlotMap;
	TArray<int32> ShopTreasureSlotIDs;
	
public:
	int32 CreateItemInstance(const FDataTableRowHandle& InItemRow, ETheOneItemType InItemType, int32 InLogicSlotID = INDEX_NONE);
	int32 CreateItemInstance(const FName& InItemRowName, ETheOneItemType InItemType, int32 InLogicSlotID = INDEX_NONE);
	void DeleteItem(int32 InItemID);
	void DeleteItemByLogicSlotID(int32 InLogicSlotID);
	void DeleteItemByLogicSlotID(TArray<int32> InLogicSlotIDs);
	void MoveItem(int32 InItemID, int32 ToSlotID);
	void RegisterCharacterSlots(uint32 CharacterAICtrlID);
	int32 RegisterOnePlayerSlot(ETheOneGridSlotType InSlotType);

	const TArray<int32>& GetShopTreasureSlotIDs() const;
	int32 FindWeaponSlotID(uint32 CharacterAICtrlID);
	int32 FindCharacterBagSlotID(uint32 CharacterAICtrlID, int InIndex);

	const FTheOneLogicSlotInfo* GetLogicSlotInfo(int32 InSlotID) const;
	

private:
	int32 IntervalCreateWeaponInstance(const FName& InWeaponRowName, const FTheOneWeaponConfig* InWeaponConfig, int32 InSlotID);
	int32 IntervalCreatePropInstance(const FName& InPropRowName, const FTheOnePropConfig* InPropConfig, int32 InSlotID);
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
