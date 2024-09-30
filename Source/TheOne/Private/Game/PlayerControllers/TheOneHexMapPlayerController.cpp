// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerControllers//TheOneHexMapPlayerController.h"

#include "Character/TheOneCharacterBase.h"
#include "Game/CursorTraceInterface.h"
#include "HexGrid/HexGrid.h"
#include "Subsystems/TheOneContextSystem.h"

void ATheOneHexMapPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	switch (FocusType) {
		case ETheOneFocusType::None:
			break;
		case ETheOneFocusType::Character:
		case ETheOneFocusType::Tile:
			{
				FocusTimer += DeltaSeconds;
				if (!Focusing && FocusTimer >= FocusTriggerTime)
				{
					Focusing = true;
					auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
					FTheOneFocusData FocusData;
					FocusData.FocusType = FocusType;
					FocusData.FocusCharacter = FocusCharacter;
					FocusData.TileIndex =  CurrentCoord.IsValid()?GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid->GetHexTileIndex(CurrentCoord) : INDEX_NONE;
					EventSystem->OnGetFocus.Broadcast(FocusData);
				}
			}
			break;
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitGround(const FVector& InHitLocation, FVector& OutGroundLocation)
{
	if (PostRealHitLocation)
	{
		OutGroundLocation = InHitLocation;
	}
	else
	{
		auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
		const double TempZ{InHitLocation.Z};
		const FHCubeCoord& HexCoord = HexGrid->WorldToHex(InHitLocation);
		FVector Result{ HexGrid->HexToWorld(HexCoord) };
		Result.Z = TempZ;
		OutGroundLocation = Result;
		if (CurrentCoord != HexCoord || FocusType != ETheOneFocusType::Tile)
		{
			FocusType = ETheOneFocusType::Tile;
			FocusCharacter = nullptr;
			CurrentCoord = HexCoord;
			FocusTimer = 0;
		}
	}
	
	if (bIsPawnImplementICursorTrace)
	{
		ICursorTraceInterface::Execute_OnHitGround(GetPawn(), OutGroundLocation);
	}
	
	if (auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid)
	{
		HexGrid->UpdateHitLocation(OutGroundLocation);
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitCharacter(ATheOneCharacterBase* HitCharacter)
{
	Super::GeneralOnHitCharacter(HitCharacter);
	if (FocusType != ETheOneFocusType::Character)
	{
		FocusType = ETheOneFocusType::Character;
		FocusCharacter = HitCharacter;
		CurrentCoord = FHCubeCoord::ErrorCoord;
		FocusTimer = 0;
	}
}

void ATheOneHexMapPlayerController::GeneralOnHitNone()
{
	if (Focusing)
	{
		Focusing = false;
		auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
		EventSystem->OnLoseFocus.Broadcast();
	}
	FocusType = ETheOneFocusType::None;

}

bool ATheOneHexMapPlayerController::CanWalk(const FVector& InLocation) const
{
	return GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid->IsTileReachable(InLocation);
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
			HexGrid->SetWireFrameColor(HexIndex,HexGrid->WireframeDefaultColor, 0.f);
		}
		TipsHexCache.Empty();
	}
}

void ATheOneHexMapPlayerController::OnSelectedCharacterEnterNewCoord(const FHCubeCoord& InCoord)
{
	auto HexGrid = GetWorld()->GetSubsystem<UTheOneContextSystem>()->HexGrid;
	for (const auto& HexIndex : TipsHexCache)
	{
		HexGrid->SetWireFrameColor(HexIndex,HexGrid->WireframeDefaultColor, 0.f);
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
