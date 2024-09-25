// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/AITask_WaitTargetArrived.h"

#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "HexGrid/HexPathFollowingComponent.h"
#include "HexGrid/HGTypes.h"

UAITask_WaitTargetArrived* UAITask_WaitTargetArrived::WaitTargetArrived(AAIController* InSourceAI,
                                                                        UAutoChessAIBBContext* InContext)
{
	if (InSourceAI == nullptr)
	{
		return nullptr;
	}

	UAITask_WaitTargetArrived* MyTask = NewAITask<UAITask_WaitTargetArrived>(*InSourceAI, EAITaskPriority::High);
	MyTask->SourceAI = CastChecked<ATheOneAutoChessAIController>(InSourceAI);
	MyTask->AIContext = InContext;
	return MyTask;
}

void UAITask_WaitTargetArrived::Activate()
{
	Super::Activate();

	if (!AIContext->WaitTargetGoingToBookedCoord)
	{
		RequestNoNeedToWait();
		return;
	}

	// auto TargetAI = AIContext->TargetAI;
	// // 检查, 存在上一帧等待的AI， 执行到这里时， 已经道达了的情况
	// auto TargetAIIsGoingToBookedCoord = TargetAI->GetCurrentHexCoord() != TargetAI->GetBookedHexCoord();
	// if (!TargetAIIsGoingToBookedCoord)
	// {
	// 	bool RequestSuccess = AIContext->TargetBookedCoord == TargetAI->GetCurrentHexCoord();
	// 	UE_LOG(LogTemp, Error, TEXT("[%s]WaitTargetArrived TargetAI: %s is not going to booked coord, Current: %s, Booked: %s, IsDead: %d, RequestSuccess: %d"),
	// 		*SourceAI->GetName(), *TargetAI->GetName(), *TargetAI->GetCurrentHexCoord().ToString(), *TargetAI->GetBookedHexCoord().ToString(), TargetAI->IsDead(), RequestSuccess);
	// 	if (RequestSuccess)
	// 	{
	// 		RequestOnTargetArrived();
	// 	}
	// 	else
	// 	{
	// 		RequestFailed();
	// 	}
	// 	return;
	// }

	// 订阅事件
	// auto HexPathCom = CastChecked<UHexPathFollowingComponent>( TargetAI->GetPathFollowingComponent());
	// SegmentFinishedDelegate = HexPathCom->OnSegmentFinishedDelegate.AddUObject(
	// 	this, &UAITask_WaitTargetArrived::OnAIArrived);
}

void UAITask_WaitTargetArrived::OnAIArrived()
{
	// auto TargetAI = AIContext->TargetAI;
	// if (SegmentFinishedDelegate.IsValid())
	// {
	//
	// 	auto HexPathCom = CastChecked<UHexPathFollowingComponent>( TargetAI->GetPathFollowingComponent());
	// 	HexPathCom->OnSegmentFinishedDelegate.Remove(SegmentFinishedDelegate);
	// 	SegmentFinishedDelegate.Reset();
	// }
	//
	// auto TargetCurrentCoord = TargetAI->GetCurrentHexCoord();
	// if (TargetCurrentCoord == AIContext->TargetBookedCoord)
	// {
	// 	RequestOnTargetArrived();
	// 	return;
	// }
	//
	// UE_LOG(LogTemp, Error, TEXT("[%s]OnTargetAI Arrived: TargetAI: %s arrive : %s, but not booked coord: %s"),
	// 	*SourceAI->GetName(), *TargetAI->GetName(), *TargetCurrentCoord.ToString(), *AIContext->TargetBookedCoord.ToString());
	RequestFailed();
}

void UAITask_WaitTargetArrived::RequestNoNeedToWait()
{
	EndTask();
	NoNeedToWait.Broadcast();
}

void UAITask_WaitTargetArrived::RequestOnTargetArrived()
{
	EndTask();
	OnTargetArrived.Broadcast();
}

void UAITask_WaitTargetArrived::RequestFailed()
{
	EndTask();
	OnRequestFailed.Broadcast();
}
