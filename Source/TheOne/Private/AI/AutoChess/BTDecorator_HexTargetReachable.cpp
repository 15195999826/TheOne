// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/BTDecorator_HexTargetReachable.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HexGrid/HexGrid.h"

UBTDecorator_HexTargetReachable::UBTDecorator_HexTargetReachable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Hex Target Reachable";

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_HexTargetReachable, BlackboardKey));
	
	bAllowAbortLowerPri = false;
	bAllowAbortNone = true;
	bAllowAbortChildNodes = false;
	FlowAbortMode = EBTFlowAbortMode::None;

	BlackboardKey.SelectedKeyName = TEXT("TargetLocation");
}

void UBTDecorator_HexTargetReachable::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_HexTargetReachable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == NULL)
	{
		return false;
	}

	// 获取BlackboardKey对应的值
	FVector TargetLocation = FVector::ZeroVector;
	const bool bHasTargetLocation = BlackboardComp->GetLocationFromEntry(BlackboardKey.GetSelectedKeyID(), TargetLocation);

	if (!bHasTargetLocation)
	{
		return false;
	}


	AHexGrid* HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(this);
	if (HexGrid == nullptr)
	{
		return false;
	}

	return HexGrid->IsTileReachable(TargetLocation);
}

FString UBTDecorator_HexTargetReachable::GetStaticDescription() const
{
	return FString::Printf(TEXT("Hex target Location reachable"));
}

#if WITH_EDITOR
FName UBTDecorator_HexTargetReachable::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.DoesPathExist.Icon");
}
#endif	// WITH_EDITOR