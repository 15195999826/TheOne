// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/BTDecorator_Hex_ActorBeSurrounded.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "HexGrid/HexGrid.h"

UBTDecorator_Hex_ActorBeSurrounded::UBTDecorator_Hex_ActorBeSurrounded(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Hex Actor Can Move";

	SelfActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_Hex_ActorBeSurrounded, SelfActorKey), AActor::StaticClass());
	AIContextKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_Hex_ActorBeSurrounded, AIContextKey), UAutoChessAIBBContext::StaticClass());

	// Todo: SkillKey
	
	bAllowAbortLowerPri = false;
	bAllowAbortNone = true;
	bAllowAbortChildNodes = false;
	FlowAbortMode = EBTFlowAbortMode::None;

	SelfActorKey.SelectedKeyName = TEXT("SelfActor");
	AIContextKey.SelectedKeyName = TEXT("Context");
}

void UBTDecorator_Hex_ActorBeSurrounded::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		SelfActorKey.ResolveSelectedKey(*BBAsset);
		AIContextKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_Hex_ActorBeSurrounded::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	// const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	// if (BlackboardComp == NULL)
	// {
	// 	return false;
	// }
	//
	// ACharacter* SelfCharacter = Cast<ACharacter>(BlackboardComp->GetValueAsObject(SelfActorKey.SelectedKeyName));
	// if (SelfCharacter == nullptr)
	// {
	// 	UE_LOG(LogTheOne, Error, TEXT("UBTDecorator_Hex_ActorCanMove::SelfCharacter is nullptr"));
	// 	return false;
	// }
	//
	// UAutoChessAIBBContext* Context = Cast<UAutoChessAIBBContext>(BlackboardComp->GetValueAsObject(AIContextKey.SelectedKeyName));
	// if (Context == nullptr)
	// {
	// 	UE_LOG(LogTheOne, Error, TEXT("UBTDecorator_Hex_ActorCanMove::Context is nullptr"));
	// 	return false;
	// }
	//
	// // 获取BlackboardKey对应的值
	// ATheOneAutoChessAIController* SelfAI = Cast<ATheOneAutoChessAIController>(SelfCharacter->GetController());
	// ATheOneAutoChessAIController* TargetAI = Context->TargetAI;
	//
	// if (SelfAI == nullptr || TargetAI == nullptr)
	// {
	// 	UE_LOG(LogTheOne, Error, TEXT("UBTDecorator_Hex_ActorCanMove::SelfAI or TargetAI is nullptr"));
	// 	return false;
	// }
	//
	// // 检查两个人的距离是否为1， 大于1则可以移动
	//
	// AHexGrid* HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(this);
	// if (HexGrid == nullptr)
	// {
	// 	return false;
	// }
	//
	// auto SelfActorHexCoord = SelfAI->GetCurrentHexCoord();
	//
	// auto NeighborHexTiles = HexGrid->GetRange(SelfActorHexCoord, 1);
	// 检查周围1格邻居是否都有人
	// 是否有缺口
	// bool HasGap = false;
	// for (const FHexTile& NeighborHexTile : NeighborHexTiles)
	// {
		// if (!NeighborHexTile.HasActor() && !NeighborHexTile.HasAIBooked)
		// {
			// HasGap = true;
			// break;
		// }
	// }

	// return !HasGap;
	return false;
}

FString UBTDecorator_Hex_ActorBeSurrounded::GetStaticDescription() const
{
	return FString::Printf(TEXT("Hex Actor Can Move"));
}


#if WITH_EDITOR
FName UBTDecorator_Hex_ActorBeSurrounded::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.DoesPathExist.Icon");
}
#endif
