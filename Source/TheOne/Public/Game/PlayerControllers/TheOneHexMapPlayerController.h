// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TheOnePlayerControllerBase.h"
#include "HexGrid/HGTypes.h"
#include "TheOneHexMapPlayerController.generated.h"

class ATheOneCharacterBase;
/**
 * 
 */
UCLASS()
class THEONE_API ATheOneHexMapPlayerController : public ATheOnePlayerControllerBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
	bool PostRealHitLocation = false;

public:
	const ATheOneCharacterBase* GetSelectedCharacter() const { return SelectedCharacter; }

	void PlayInEditor() { bPlayInEditor = true; }

	UFUNCTION(BlueprintCallable)
	bool IsPlayInEditor() const { return bPlayInEditor; }
	
protected:
	virtual void GeneralOnHitGround(const FVector& InHitLocation) override;
	virtual FVector GroundLocationHook(const FVector& InLocation) override;

	virtual void ShowReleaseDistanceTips() override;
	
	virtual void HideReleaseDistanceTips() override;

private:
	bool bPlayInEditor = false;

	bool HasListenEnterNewCoord = false;
	TArray<int> TipsHexCache;
	UFUNCTION()
	void OnSelectedCharacterEnterNewCoord(const FHCubeCoord& InCoord);

};
