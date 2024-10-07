// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModeComponents/TheOneEquipItemComponent.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "Item/TheOneItemSystem.h"

// Sets default values for this component's properties
UTheOneEquipItemComponent::UTheOneEquipItemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTheOneEquipItemComponent::BeginPlay()
{
	Super::BeginPlay();

	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	ItemSystem->OnItemCreated.AddUObject(this, &UTheOneEquipItemComponent::OnItemCreated);
	ItemSystem->OnItemDeleted.AddUObject(this, &UTheOneEquipItemComponent::OnItemDeleted);
	ItemSystem->OnItemUpdated.AddUObject(this, &UTheOneEquipItemComponent::OnItemUpdated);
}

void UTheOneEquipItemComponent::OnItemCreated(const FTheOneItemInstance& InItemInstance)
{
	OnItemUpdated(INDEX_NONE, InItemInstance);
}

void UTheOneEquipItemComponent::OnItemDeleted(const FTheOneItemInstance& InItemInstance)
{
}

void UTheOneEquipItemComponent::OnItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance)
{
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	if (OldSlotID != INDEX_NONE)
	{
		const auto& OldSlotInfo = ItemSystem->GetLogicSlotInfo(OldSlotID);
		if (OldSlotInfo->SlotType == ETheOneGridSlotType::CharacterBag)
		{
			auto OldCharacter = UTheOneBlueprintFunctionLibrary::FindCharacter(this, OldSlotInfo->OwnerFlag);
			check(OldCharacter);
			auto SlotType = ItemSystem->SlotID2CharacterBagSlotType(OldSlotInfo->OwnerFlag, OldSlotID);
			UTheOneBlueprintFunctionLibrary::Equip(OldCharacter, INDEX_NONE, SlotType);
		}
	}

	auto SlotInfo = ItemSystem->GetLogicSlotInfo(InItemInstance.LogicSlotID);
	check(SlotInfo);
	switch (SlotInfo->SlotType) {
		case ETheOneGridSlotType::CharacterBag:
			{
				auto Character = UTheOneBlueprintFunctionLibrary::FindCharacter(this, SlotInfo->OwnerFlag);
				check(Character);
				auto SlotType = ItemSystem->SlotID2CharacterBagSlotType(SlotInfo->OwnerFlag, InItemInstance.LogicSlotID);
				UTheOneBlueprintFunctionLibrary::Equip(Character, InItemInstance.ItemID, SlotType);
			}
			break;
		default:
			break;
	}
}


