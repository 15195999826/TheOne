// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAbilityWidget.h"

#include "Components/Image.h"

void UTheOneAbilityWidget::BindAbilityData(UTheOneGeneralGA* GA)
{
	auto ConfigRow = GA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("AbilityWidget.BindData");
	if(ConfigRow)
	{
		Icon->SetBrushFromTexture(ConfigRow->Icon.LoadSynchronous());
	}

	Execute_BindShortcutKey(this);
}

void UTheOneAbilityWidget::UnBindAbilityData()
{
	Execute_UnBindShortcutKey(this);
}
