// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneBattleEvaluate.generated.h"
class ATheOneCharacterBase;
class ATheOneBattle;

USTRUCT(BlueprintType)
struct FTheOneEvaParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ATheOneCharacterBase* Self;
	UPROPERTY(BlueprintReadOnly)
	ATheOneCharacterBase* Target;
	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;
	UPROPERTY(BlueprintReadOnly)
	bool SelfInAnyOpponentZOC;
	UPROPERTY(BlueprintReadOnly)
	int HealthSortIndex;
	UPROPERTY(BlueprintReadOnly)
	int32 AbilityIndex;
};

/**
 * Todo: 根据Memory, 与记忆中相同目标的行为，额外得分
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
	double CallEvaFunctionByName(const FName& InFunctionName, const FTheOneEvaParams& InParams);

	double NativeEndTurn(ATheOneCharacterBase* InSelf);
	double NativeWaitTurn(ATheOneCharacterBase* InSelf);
	double NativeRashMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, int HealthSortIndex, FVector& OutTargetLocation);
	double NativeConservativeMove(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget, bool InAnyOpponentZOC, int HealthSortIndex,FVector& OutTargetLocation);

	/**
	 * 通用伤害型技能评估
	 */
	UFUNCTION(BlueprintCallable)
	double GeneralSingleMelee(const FTheOneEvaParams& InParams);

protected:
	bool HasAnyTeammateInTargetZOC(ATheOneCharacterBase* InSelf, ATheOneCharacterBase* InTarget);
	
	// float Get2OpponentAbilityMinCost(ATheOneCharacterBase* InSelf);
};
