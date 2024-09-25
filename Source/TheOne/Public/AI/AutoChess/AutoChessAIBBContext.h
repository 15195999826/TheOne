// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HexGrid/HGTypes.h"
#include "UObject/NoExportTypes.h"
#include "AutoChessAIBBContext.generated.h"

class ATheOneAutoChessAIController;
/**
 * 存放BlackBoard中不好存的数据，但是行为树的运行不依赖Context中的数据
 */
UCLASS(BlueprintType)
class THEONE_API UAutoChessAIBBContext : public UObject
{
	GENERATED_BODY()

	friend ATheOneAutoChessAIController;

public:
	inline static FName TargetIsDeadKey = "TargetIsDead";
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ATheOneAutoChessAIController> TargetAI;

	UPROPERTY(BlueprintReadWrite)
	bool ForceRetarget = false;

	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilitySpecHandle SelectedAbilitySpecHandle;

	UPROPERTY(BlueprintReadWrite)
	bool WaitTargetGoingToBookedCoord = false;

	UPROPERTY(BlueprintReadWrite)
	FHCubeCoord TargetBookedCoord;

public:
	UFUNCTION(BlueprintCallable)
	bool CanReleaseAbility(bool UseTargetBookedCoord);

	void SetTargetAI(ATheOneAutoChessAIController* InTargetAI);

private:
	FDelegateHandle TargetDeadHandle;

	void OnTargetDead();
};
