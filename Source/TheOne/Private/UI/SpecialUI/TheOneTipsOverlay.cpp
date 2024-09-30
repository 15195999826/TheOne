// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneTipsOverlay.h"

#include "Game/TheOneEventSystem.h"

void UTheOneTipsOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnGetFocus.AddUObject(this, &UTheOneTipsOverlay::OnGetFocus);
	EventSystem->OnLoseFocus.AddUObject(this, &UTheOneTipsOverlay::OnLoseFocus);
}
