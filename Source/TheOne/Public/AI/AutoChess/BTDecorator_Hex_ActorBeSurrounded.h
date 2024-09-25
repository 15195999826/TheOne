// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Hex_ActorBeSurrounded.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UBTDecorator_Hex_ActorBeSurrounded : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

protected:
	/** blackboard key selector */
	UPROPERTY(EditAnywhere, Category=Condition)
	FBlackboardKeySelector SelfActorKey;

	UPROPERTY(EditAnywhere, Category=Condition)
	FBlackboardKeySelector AIContextKey;

public:
	THEONE_API virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	THEONE_API virtual FString GetStaticDescription() const override;
	THEONE_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
	THEONE_API virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR
};
