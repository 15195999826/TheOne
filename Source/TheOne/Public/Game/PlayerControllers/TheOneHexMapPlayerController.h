// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AutoChess/AITask_AutoChessMoveTo.h"
#include "Game/TheOnePlayerControllerBase.h"
#include "HexGrid/HGTypes.h"
#include "Types/TheOneFocusData.h"
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

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> ActiveCharacter;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AAIController> ActiveAI;

public:
	const ATheOneCharacterBase* GetSelectedCharacter() const { return SelectedCharacter; }

	void PlayInEditor() { bPlayInEditor = true; }

	UFUNCTION(BlueprintCallable)
	bool IsPlayInEditor() const { return bPlayInEditor; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GeneralOnHitGround(const FVector& InHitLocation, FVector& OutGroundLocation) override;
	virtual void GeneralOnHitCharacter(ATheOneCharacterBase* HitCharacter) override;
	virtual void GeneralOnHitNone() override;
	
	virtual bool CanWalk(const FVector& InLocation) const override;

	virtual void ShowReleaseDistanceTips() override;
	
	virtual void HideReleaseDistanceTips() override;
	void HideAllPathTips();

	virtual void BP_OnHitGround_Implementation(const FVector& HitLocation, bool bIsRightClick, bool bIsLeftClick, bool CanWalk) override;
private:
	bool bPlayInEditor = false;

	bool HasListenEnterNewCoord = false;
	TArray<int> TipsHexCache;
	UFUNCTION()
	void OnSelectedCharacterEnterNewCoord(const FHCubeCoord& InCoord);

	// For Focus
protected:
	UPROPERTY(EditAnywhere)
	float FocusTriggerTime = 0.5f;
private:
	ETheOneFocusType FocusType = ETheOneFocusType::None;
	FHCubeCoord FocusCurrentCoord;
	TWeakObjectPtr<ATheOneCharacterBase> FocusCharacter;
	float FocusTimer = 0.0f;
	bool Focusing = false;

	// For PathFinding
protected:
	bool CanPlayerControl = false;
	bool IsCommanding = false;
	FVector DesiredGoalLocation;
	FVector CanArriveGoalLocation;
	float CurrentCost = 0;
	FAIMoveRequest MoveRequest;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PathTipClass;
	UPROPERTY()
	TArray<AActor*> ShowingPathTips;
	UPROPERTY()
	TArray<AActor*> PathTipPool;
	AActor* GetPathTip();
	void ReleasePathTip(AActor* InTip);

public:
	bool FindPath(AAIController* InAI, ATheOneCharacterBase* InCharacter,
											 const FVector& GoalLocation, FVector& OutDesiredGoalLocation,
											 float& OutCost, FVector& OutCanArriveGoalLocation, TArray<FNavPathPoint>& OutPath, int& OutCanMoveCount);
	// For UseAbilityCommand
protected:
	virtual void ReceiveUseAbilityCommand(const FTheOneUseAbilityCommandPayload& Payload) override;
	UFUNCTION()
	void OnCommandBehaviorEnd(bool bSuccess);
	virtual void OnAbilityCommandCanceled() override;
	virtual void OnAbilityCommandFinished() override;
	virtual bool InReleaseDistance(const FVector& SourceLocation, const FVector& TargetLocation, int InReleaseDistance) override;
	virtual void ConsumeActionPoint(ATheOneCharacterBase* InCharacter, float InCost) override;
	// For Turn
protected:
	virtual ATheOneCharacterBase* GetExecCharacter() override;
	
	void OnCharacterGetTurn(ATheOneCharacterBase* TheOneCharacterBase);
	
	UFUNCTION(BlueprintCallable)
	void EndActiveCharacterTurn();
	
};
