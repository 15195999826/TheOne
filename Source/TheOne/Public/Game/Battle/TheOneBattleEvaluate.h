// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneBattleEvaluate.generated.h"

class ATheOneCharacterBase;
class ATheOneBattle;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEONE_API ATheOneBattleEvaluate : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ATheOneBattleEvaluate();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	double CallEvaFunctionByName(const FName& InFunctionName, ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, const FVector& InTargetLocation);

	double NativeEndTurn(ATheOneCharacterBase* InSelf);
	double NativeWaitTurn(ATheOneCharacterBase* InSelf);
	double NativeRashMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, bool TargetIsOppTeamMinHealth, FVector& OutTargetLocation);
	double NativeConservativeMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, bool TargetIsOppTeamMinHealth,FVector& OutTargetLocation);

protected:
	bool HasAnyTeamateInTargetZOC(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget);
	
	// float Get2OpponentAbilityMinCost(ATheOneCharacterBase* InSelf);
};
