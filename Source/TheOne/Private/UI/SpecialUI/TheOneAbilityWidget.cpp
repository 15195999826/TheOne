// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAbilityWidget.h"

#include "Components/Image.h"

void UTheOneAbilityWidget::BindData(UTheOneGeneralGA* GA)
{
	if(GA == nullptr)
	{
		return;
	}

	auto ConfigRow = GA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("AbilityWidget.BindData");
	if(ConfigRow)
	{
		Icon->SetBrushFromTexture(ConfigRow->Icon.LoadSynchronous());
	}
}
