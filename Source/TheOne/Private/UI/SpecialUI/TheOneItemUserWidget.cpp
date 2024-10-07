// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneItemUserWidget.h"

#include "Game/TheOneEventSystem.h"

void UTheOneItemUserWidget::Clear_Implementation()
{
}


void UTheOneItemUserWidget::SetupEquipmentConfig_Implementation(bool IsPreview, const FName& InRowName,
	const FTheOneEquipmentConfig& EquipmentConfig, int InItemID)
{
	bIsPreview = IsPreview;
	RowName = InRowName;
	ItemID = InItemID;
	ItemType = ETheOneItemType::Equipment;
	if (!bIsPreview)
	{
		check(ItemID > 0);
	}
}

void UTheOneItemUserWidget::SetupMinionConfig_Implementation(bool IsPreview, FName MinionTemplateName, int32 InItemID)
{
	bIsPreview = IsPreview;
	RowName = MinionTemplateName;
	ItemID = InItemID;
	ItemType = ETheOneItemType::Minion;
	if (!bIsPreview)
	{
		check(ItemID > 0);
	}
}

FReply UTheOneItemUserWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 检查是否是左键点击
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (ItemType == ETheOneItemType::Minion)
		{
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->OnSelectInBagCharacter.Broadcast(ItemID);
		}
	}
	
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
