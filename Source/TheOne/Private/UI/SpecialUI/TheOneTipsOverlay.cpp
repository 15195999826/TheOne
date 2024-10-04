// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneTipsOverlay.h"

#include "Game/TheOneEventSystem.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"

void UTheOneTipsOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnGetFocus.AddUObject(this, &UTheOneTipsOverlay::OnGetFocus);
	EventSystem->OnLoseFocus.AddUObject(this, &UTheOneTipsOverlay::OnLoseFocus);
}

void UTheOneTipsOverlay::OnGetFocus(const FTheOneFocusData& TheOneFocusData)
{
	PostGetNewFocus();
	switch (TheOneFocusData.FocusType)
	{
		case ETheOneFocusType::None:
			break;
		case ETheOneFocusType::Character:
			break;
		case ETheOneFocusType::Tile:
			{
				auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
				check(HexGrid->GridTiles.IsValidIndex(TheOneFocusData.TileIndex));
				const auto& Tile = HexGrid->GridTiles[TheOneFocusData.TileIndex];
				const FHTileEnvironment& EnvData = HexGrid->Environments[Tile.EnvironmentType];
				PostFocusTile(EnvData.EnvironmentName, Tile.Cost, 0, Tile.bIsBlocking);
			}
			break;
	}
	
}