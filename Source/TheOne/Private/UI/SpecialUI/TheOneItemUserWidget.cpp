// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneItemUserWidget.h"

void UTheOneItemUserWidget::Clear_Implementation()
{
}

void UTheOneItemUserWidget::SetupWeaponConfig_Implementation(bool IsPreview, const FName& InRowName, const FTheOneWeaponConfig& WeaponConfig,
	int InItemID)
{
	bIsPreview = IsPreview;
	RowName = InRowName;
	ItemID = InItemID;
	ItemType = ETheOneItemType::Weapon;
	if (!bIsPreview)
	{
		check(ItemID > 0);
	}
}