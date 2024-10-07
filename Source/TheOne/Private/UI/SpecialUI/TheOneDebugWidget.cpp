// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneDebugWidget.h"

#include "Item/TheOneItemSystem.h"
#include "Subsystems/TheOneTeamSystem.h"

void UTheOneDebugWidget::AddMinion(const FName& MinionRowName)
{
	auto TeamSystem = GetWorld()->GetSubsystem<UTheOneTeamSystem>();
	TeamSystem->AddCharacterToTeam(TeamSystem->GetPlayerTeamID(), MinionRowName, true);
}

void UTheOneDebugWidget::AddEquipment(const FName& EquipmentRowName)
{
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	// Find
	auto FirstEmptySlot = ItemSystem->FindFirstEmptyPlayerBagSlotID();
	if (FirstEmptySlot == INDEX_NONE)
	{
		return;
	}
	
	ItemSystem->CreateItemInstance(EquipmentRowName, ETheOneItemType::Equipment, FirstEmptySlot);
}
