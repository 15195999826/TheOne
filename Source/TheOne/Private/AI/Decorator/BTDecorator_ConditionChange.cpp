// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_ConditionChange.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_ConditionChange::UBTDecorator_ConditionChange(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Condition Change";
}

bool UBTDecorator_ConditionChange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	// Todo: 功能设定： 监听BBContext中某值发生变化
	return true;
}

EBlackboardNotificationResult UBTDecorator_ConditionChange::OnBlackboardKeyValueChange(
	const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*)Blackboard.GetBrainComponent();
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}
	
	if (BlackboardKey.GetSelectedKeyID() == ChangedKeyID)
	{
		DoOnce = true;
		// can't simply use BehaviorComp->RequestExecution(this) here, we need to support condition/value change modes
		ConditionalFlowAbort(*BehaviorComp,  EBTDecoratorAbortRequest::ConditionPassing);
	}
	
	return EBlackboardNotificationResult::ContinueObserving;
}
