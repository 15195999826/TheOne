// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"

#include "BTDecorator_ConditionChange.generated.h"

/**
 * 我需要的功能是: 当条件改变时， 打断当前的行为树节点，重新执行节点功能， 当节点执行完毕后，只要条件不变，就不再执行
 */
UCLASS()
class THEONE_API UBTDecorator_ConditionChange : public UBTDecorator_BlackboardBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;

private:
	bool DoOnce = false;
};
