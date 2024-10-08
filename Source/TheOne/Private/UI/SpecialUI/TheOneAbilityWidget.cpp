// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAbilityWidget.h"

#include "Components/Image.h"

void UTheOneAbilityWidget::BindAbilityData(UTheOneGeneralGA* GA, int InPayload)
{
	auto ConfigRow = GA->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("AbilityWidget.BindData");
	if(ConfigRow)
	{
		Icon->SetBrushFromTexture(ConfigRow->Icon.LoadSynchronous());
	}
	IntPayload = InPayload;
	ShortcutKey = KeyIndex2KeyMap[InPayload];

	Execute_BindShortcutKey(this);
}

void UTheOneAbilityWidget::UnBindAbilityData()
{
	Execute_UnBindShortcutKey(this);
}
