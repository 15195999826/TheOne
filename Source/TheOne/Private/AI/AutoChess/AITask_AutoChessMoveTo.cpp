// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/AITask_AutoChessMoveTo.h"

#include "AIResources.h"
#include "AISystem.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "HexGrid/HexGrid.h"
#include "HexGrid/HGTypes.h"

UAITask_AutoChessMoveTo::UAITask_AutoChessMoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
	MoveRequestID = FAIRequestID::InvalidRequest;

	MoveRequest.SetAcceptanceRadius(GET_AI_CONFIG_VAR(AcceptanceRadius));
	MoveRequest.SetReachTestIncludesAgentRadius(GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap));
	MoveRequest.SetAllowPartialPath(GET_AI_CONFIG_VAR(bAcceptPartialPaths));
	MoveRequest.SetUsePathfinding(true);

	AddRequiredResource(UAIResource_Movement::StaticClass());
	AddClaimedResource(UAIResource_Movement::StaticClass());
}

UAITask_AutoChessMoveTo* UAITask_AutoChessMoveTo::AutoChessMoveTo(AAIController* InSourceAI,
                                                                  const FBlackboardKeySelector& InTargetActorBlackboardKey,
                                                                  const FBlackboardKeySelector& InTargetLocationBlackboardKey,
                                                                  const FBlackboardKeySelector& InGoalDistanceBlackboardKey,
                                                                  float InAcceptanceRadius,
                                                                  EAIOptionFlag::Type InAcceptPartialPath,
                                                                  EAIOptionFlag::Type InRequireNavigableEndLocation,
                                                                  EAIOptionFlag::Type InProjectGoalOnNavigation,
                                                                  bool InAllowStrafe,
                                                                  bool InReachTestIncludesAgentRadius,
                                                                  bool InReachTestIncludesGoalRadius,
                                                                  bool InbLockAILogic)
{
	if (InSourceAI == nullptr)
	{
		return nullptr;
	}
	
	FAIMoveRequest MoveReq;
	MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(InAcceptPartialPath, MoveReq.IsUsingPartialPaths()));
	MoveReq.SetAcceptanceRadius(InAcceptanceRadius);
	MoveReq.SetCanStrafe(InAllowStrafe);
	MoveReq.SetReachTestIncludesAgentRadius(InReachTestIncludesAgentRadius);
	MoveReq.SetReachTestIncludesGoalRadius(InReachTestIncludesGoalRadius);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetRequireNavigableEndLocation(FAISystem::PickAIOption(InRequireNavigableEndLocation, MoveReq.IsNavigableEndLocationRequired()));
	MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(InProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
	MoveReq.SetNavigationFilter(InSourceAI->GetDefaultNavigationFilterClass());
	
	//创建Task对象
	UAITask_AutoChessMoveTo* MyTask = NewAITask<UAITask_AutoChessMoveTo>(*InSourceAI, EAITaskPriority::High);
	MyTask->OwnerController = InSourceAI;
	
	//设置基本属性
	MyTask->WeakOwnerHexController = CastChecked<ATheOneAutoChessAIController>(InSourceAI);
	
	if (MyTask->WeakOwnerHexController.IsValid())
	{
		auto Pawn = MyTask->WeakOwnerHexController->GetPawn();
		if (Pawn && Pawn->Implements<UInHexActorInterface>())
		{
			MyTask->InHexActorInterface = TScriptInterface<IInHexActorInterface>(Pawn);
		}
	}
	MyTask->TargetActorBlackboardKeyName = InTargetActorBlackboardKey.SelectedKeyName;
	MyTask->TargetActorBlackboardKeyID = InTargetActorBlackboardKey.GetSelectedKeyID();
	MyTask->TargetLocationBlackboardKeyName = InTargetLocationBlackboardKey.SelectedKeyName;
	MyTask->TargetLocationBlackboardKeyID = InTargetLocationBlackboardKey.GetSelectedKeyID();
	MyTask->GoalDistanceBlackboardKeyName = InGoalDistanceBlackboardKey.SelectedKeyName;
	MyTask->GoalDistanceBlackboardKeyID = InGoalDistanceBlackboardKey.GetSelectedKeyID();
	MyTask->MoveRequest = MoveReq;
	if (InbLockAILogic)
	{
		MyTask->RequestAILogicLocking();
	}
	
	return MyTask;
}

EBlackboardNotificationResult UAITask_AutoChessMoveTo::OnTargetActorValueChange(
	const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key)
{
	TargetActor = Cast<AActor>(BlackboardComponent.GetValue<UBlackboardKeyType_Object>(TargetActorBlackboardKeyName));
	return EBlackboardNotificationResult::ContinueObserving;
}

EBlackboardNotificationResult UAITask_AutoChessMoveTo::OnTargetLocationValueChange(
	const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key)
{
	auto TargetLocation = BlackboardComponent.GetValue<UBlackboardKeyType_Vector>(TargetLocationBlackboardKeyName);
	GoalCoord = UTheOneBlueprintFunctionLibrary::GetHexGrid(WeakOwnerHexController.Get())->WorldToHex(TargetLocation);
	return EBlackboardNotificationResult::ContinueObserving;
}

EBlackboardNotificationResult UAITask_AutoChessMoveTo::OnGoalDistanceValueChange(
	const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key)
{
	GoalDistance = BlackboardComponent.GetValueAsInt(GoalDistanceBlackboardKeyName);
	return EBlackboardNotificationResult::ContinueObserving;
}

void UAITask_AutoChessMoveTo::OnDestroy(bool InbOwnerFinished)
{
	UE_LOG(LogTheOneAI, Log, TEXT("UAITask_AutoChessMoveTo::Destroy, Task: %s"), *GetName());
	WeakOwnerHexController->GetBlackboardComponent()->UnregisterObserver(TargetActorBlackboardKeyID, ActorBBKeyChangeHandle);
	WeakOwnerHexController->GetBlackboardComponent()->UnregisterObserver(TargetLocationBlackboardKeyID, LocationBBKeyChangeHandle);
	
	Super::OnDestroy(InbOwnerFinished);
}

//触发任务
void UAITask_AutoChessMoveTo::Activate()
{
	UE_LOG(LogTheOneAI, Log, TEXT("UAITask_AutoChessMoveTo::Activate, Task: %s"), *GetName());
	Super::Activate();
	MoveRequestID = FAIRequestID::InvalidRequest;
	auto BlackboardComponent = WeakOwnerHexController->GetBlackboardComponent();
	TargetActor = Cast<AActor>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(TargetActorBlackboardKeyName));
	auto TargetLocation = BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(TargetLocationBlackboardKeyName);
	GoalCoord = UTheOneBlueprintFunctionLibrary::GetHexGrid(WeakOwnerHexController.Get())->WorldToHex(TargetLocation);
	GoalDistance = BlackboardComponent->GetValueAsInt(GoalDistanceBlackboardKeyName);
	ActorBBKeyChangeHandle = BlackboardComponent->RegisterObserver(TargetActorBlackboardKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UAITask_AutoChessMoveTo::OnTargetActorValueChange));
	LocationBBKeyChangeHandle = BlackboardComponent->RegisterObserver(TargetLocationBlackboardKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UAITask_AutoChessMoveTo::OnTargetLocationValueChange));
	GoalDistanceBBKeyChangeHandle = BlackboardComponent->RegisterObserver(GoalDistanceBlackboardKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UAITask_AutoChessMoveTo::OnGoalDistanceValueChange));
	//触发任务后开始移动
	ConditionalPerformMove();
}

void UAITask_AutoChessMoveTo::ConditionalPerformMove()
{
	if (MoveRequest.IsUsingPathfinding() && OwnerController && OwnerController->ShouldPostponePathUpdates())
	{
		UE_LOG(LogTheOneAI, Verbose, TEXT(" Actor: %s, can't path right now, waiting..."), *OwnerController->GetPawn()->GetName());
		OwnerController->GetWorldTimerManager().SetTimer(MoveRetryTimerHandle, this, &UAITask_AutoChessMoveTo::ConditionalPerformMove, 0.2f, false);
	}
	else
	{
		MoveRetryTimerHandle.Invalidate();
		PerformMove();
	}
}

void UAITask_AutoChessMoveTo::PerformMove()
{
	if (!WeakOwnerHexController.IsValid() || InHexActorInterface.GetObject() == nullptr)
	{
		RequestFailed();
		return;
	}
	
	if (OwnerController)
	{
		// Remove all timers including the ones that might have been set with SetTimerForNextTick 
		OwnerController->GetWorldTimerManager().ClearAllTimersForObject(this);
	}

	if (bIsAborting)
	{
		RequestAbort();
		return;
	}
	
	// if (TargetAI->IsDead())
	// {
	// 	RequestFailed();
	// 	return;
	// }
	FHCubeCoord TargetCoord;
	auto HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(WeakOwnerHexController.Get());
	bool TargetIsGoingToBookedCoord = false;
	const FHCubeCoord& SourceAICoord = InHexActorInterface->GetCurrentHexCoord();
	int ProtectPathPointNum = 2;
	if (TargetActor.IsValid() && TargetActor->Implements<UInHexActorInterface>())
	{
		IInHexActorInterface* TargetAI = Cast<IInHexActorInterface>(TargetActor);
		if (!TargetAI)
		{
			RequestFailed();
			return;
		}
		
		
		const FHCubeCoord& TargetAICoord = TargetAI->GetCurrentHexCoord();
		const FHCubeCoord& TargetAIBookedCoord = TargetAI->GetBookedHexCoord();
		
		if (TargetAICoord != TargetAIBookedCoord)
		{
			TargetCoord = TargetAIBookedCoord;
			TargetIsGoingToBookedCoord = true;
			UE_LOG(LogTheOneAI, Verbose, TEXT("[Actor %s]Use Booked Coord SourceAI: %s, TargetAI: %s"), *OwnerController->GetPawn()->GetName(), *SourceAICoord.ToString(), *TargetAIBookedCoord.ToString());
		}
		else
		{
			TargetCoord = TargetAICoord;
			UE_LOG(LogTheOneAI, Verbose, TEXT("Use Current Coord SourceAI: %s, TargetAI: %s"), *SourceAICoord.ToString(), *TargetAICoord.ToString());
		}
		ProtectPathPointNum = 3;
	}
	else
	{
		TargetCoord = GoalCoord;
	}

	// 判断距离是否满足GoalDistance
	auto Distance = HexGrid->GetDistance(SourceAICoord, TargetCoord);
	UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, Distance: %d, GoalDistance: %d"), *OwnerController->GetPawn()->GetName(), Distance, GoalDistance);
	if (Distance <= GoalDistance)
	{
		RequestSuccess(TargetIsGoingToBookedCoord, TargetCoord);
		return;
	}
	
	auto NeighborHexTiles = HexGrid->GetRange(SourceAICoord, 1);
	// 检查周围1格邻居是否都有人
	// 是否有缺口
	bool HasGap = false;
	for (const FHexTile& NeighborHexTile : NeighborHexTiles)
	{
		if (!NeighborHexTile.HasActor() && !NeighborHexTile.HasAIBooked)
		{
			HasGap = true;
			break;
		}
	}

	if (!HasGap)
	{
		// 没有可以移动的缺口，并且无法释放技能
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, 被棋子包围无法移动"), *OwnerController->GetPawn()->GetName());
		RequestFailed();
		return;
	}

	
	UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
	if (PFComp == nullptr)
	{
		RequestFailed();
		return;
	}
	
	MoveRequest.UpdateGoalLocation(HexGrid->HexToWorld(TargetCoord));
	
	// start new move request
	FPathFindingQuery PFQuery;
	const bool bValidQuery = OwnerController->BuildPathfindingQuery(MoveRequest, PFQuery);
	if (!bValidQuery)
	{
		RequestFailed();
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, bValidQuery is not valid"), *OwnerController->GetPawn()->GetName());
		return;
	}
	FNavPathSharedPtr FollowedPath;
	OwnerController->FindPathForMoveRequest(MoveRequest, PFQuery, FollowedPath);

	if (!FollowedPath.IsValid())
	{
		RequestFailed();
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, FollowedPath is not valid"), *OwnerController->GetPawn()->GetName());
		return;
	}

	// 理论上路径上至少有2个点
	if (FollowedPath->GetPathPoints().Num() < ProtectPathPointNum)
	{
#if WITH_EDITOR || UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		auto PathPoints = FollowedPath->GetPathPoints();
		FString PathPointsStr;
		for (const auto& PathPoint : PathPoints)
		{
			PathPointsStr.Append("{");
			PathPointsStr.Append(PathPoint.Location.ToString());
			PathPointsStr.Append("}");
		}
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, PathPointsNum < %d, PathPoints: %s"), *OwnerController->GetPawn()->GetName(), ProtectPathPointNum, *PathPointsStr);
#endif
		// Todo: FinishTask
		RequestFailed();
		return;
	}
	
	// Modify the path, only maintain one step		
	FollowedPath->GetPathPoints().SetNum(2);
	UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, PathNum: %d, FirstPoint: %s, SecondPoint: %s"),
	       *OwnerController->GetPawn()->GetName(), FollowedPath->GetPathPoints().Num(),
	       *FollowedPath->GetPathPoints()[0].Location.ToString(),
	       *FollowedPath->GetPathPoints()[1].Location.ToString()); 

	MoveRequest.UpdateGoalLocation(FollowedPath->GetPathPoints()[1].Location);
	
	// Begin Move To
	const FAIRequestID RequestID = OwnerController->RequestMove(MoveRequest, FollowedPath);

	if (!RequestID.IsValid())
	{
		MoveRequestID = OwnerController->GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
		RequestFailed();
		UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, RequestID is not valid"), *OwnerController->GetPawn()->GetName());
		return;
	}
	
	OwnerController->bAllowStrafe = MoveRequest.CanStrafe();
	MoveRequestID = RequestID;
	PathFinishDelegateHandle = PFComp->OnRequestFinished.AddUObject(this, &UAITask_AutoChessMoveTo::OnRequestFinished);

	if (IsFinished())
	{
		UE_LOG(LogGameplayTasks, Error, TEXT("UAITask_MoveTo::re-Activating Finished task!"));
	}
}

void UAITask_AutoChessMoveTo::OnRequestFinished(FAIRequestID RequestID,
                                                const FPathFollowingResult& Result)
{
	if (PathFinishDelegateHandle.IsValid())
	{
		UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
		if (PFComp)
		{
			PFComp->OnRequestFinished.Remove(PathFinishDelegateHandle);
		}

		PathFinishDelegateHandle.Reset();
	}
	
	if (RequestID == MoveRequestID)
	{
		if (Result.HasFlag(FPathFollowingResultFlags::UserAbort) && Result.HasFlag(FPathFollowingResultFlags::NewRequest) && !Result.HasFlag(FPathFollowingResultFlags::ForcedScript))
		{
			UE_LOG(LogTheOneAI, Verbose, TEXT("Actor: %s, ignoring OnRequestFinished, move was aborted by new request"), *OwnerController->GetPawn()->GetName());
		}
		else
		{
			// reset request Id, FinishMoveTask doesn't need to update path following's state
			MoveRequestID = FAIRequestID::InvalidRequest;
			// 递归PerformMove, 直到到达目标距离
			PerformMove();
		}
	}
	else if (IsActive())
	{
		UE_LOG(LogTheOne, Error, TEXT("Actor: %s, received OnRequestFinished with not matching RequestID!"), *OwnerController->GetPawn()->GetName());
	}
}

void UAITask_AutoChessMoveTo::FinishMoveTask()
{
	if (MoveRequestID.IsValid())
	{
		UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
		if (PFComp && PFComp->GetStatus() != EPathFollowingStatus::Idle)
		{
			PFComp->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MoveRequestID);
		}
	}
}

void UAITask_AutoChessMoveTo::RequestSuccess(bool TargetIsGoingToBooked, const FHCubeCoord& InTargetBookedCoord)
{
	FinishMoveTask();
	EndTask();
	OnRequestSuccess.Broadcast(TargetIsGoingToBooked, InTargetBookedCoord);
}

void UAITask_AutoChessMoveTo::RequestFailed()
{
	FinishMoveTask();
	EndTask();
	OnRequestFailed.Broadcast();
}

void UAITask_AutoChessMoveTo::AbortTask()
{
	bIsAborting = true;
}

void UAITask_AutoChessMoveTo::SimpleEndTask()
{
	if (PathFinishDelegateHandle.IsValid())
	{
		UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
		if (PFComp)
		{
			PFComp->OnRequestFinished.Remove(PathFinishDelegateHandle);
		}

		PathFinishDelegateHandle.Reset();
	}
	EndTask();
}

void UAITask_AutoChessMoveTo::RequestAbort()
{
	FinishMoveTask();
	EndTask();
	OnAbort.Broadcast();
}
