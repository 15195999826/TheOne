// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerControllers//TheOneHexMapPlayerController.h"

#include "Character/TheOneCharacterBase.h"
#include "Game/CursorTraceInterface.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"

void ATheOneHexMapPlayerController::GeneralOnHitGround(const FVector& InHitLocation)
{
	Super::GeneralOnHitGround(InHitLocation);
	if (auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid)
	{
		HexGrid->UpdateHitLocation(InHitLocation);
	}
}

FVector ATheOneHexMapPlayerController::GroundLocationHook(const FVector& InLocation)
{
	if (PostRealHitLocation)
	{
		return InLocation;
	}
	
	if (auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid)
	{
		return HexGrid->SnapToGrid(InLocation);
	}

	return InLocation;
}

void ATheOneHexMapPlayerController::ShowReleaseDistanceTips()
{
	if (SelectedCharacter)
    {
		OnSelectedCharacterEnterNewCoord(SelectedCharacter->GetCurrentHexCoord());
		if  (!HasListenEnterNewCoord)
		{
			HasListenEnterNewCoord = true;
			// 监听SelectedCharacter的位置更新
			SelectedCharacter->OnEnterNewCoord.AddDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		}
    }
}

void ATheOneHexMapPlayerController::HideReleaseDistanceTips()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->OnEnterNewCoord.RemoveDynamic(this, &ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord);
		HasListenEnterNewCoord = false;
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		for (const auto& HexIndex : TipsHexCache)
		{
			// HexGrid->SetHexTileColor(HexIndex,HexGrid->DefaultColor, 0.f);
		}
		TipsHexCache.Empty();
	}
}

void ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord(const FHCubeCoord& InCoord)
{
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	for (const auto& HexIndex : TipsHexCache)
	{
		// HexGrid->SetHexTileColor(HexIndex,HexGrid->DefaultColor, 0.f);
	}
	TipsHexCache.Empty();
	auto CurrentCoord = SelectedCharacter->GetCurrentHexCoord();
	auto Hexes = HexGrid->GetRange(CurrentCoord, ReleaseDistanceCache);
	for (const auto& Hex : Hexes)
	{
		auto MeshInstanceIndex = HexGrid->GetHexTileIndex(Hex.CubeCoord);
		HexGrid->SetWireFrameColor(MeshInstanceIndex, FLinearColor::Green, 0.3f);
		TipsHexCache.Add(MeshInstanceIndex);
	}
}
