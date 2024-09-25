// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTDecorator_Hex_ActorBeSurrounded.h"
#include "HexGrid/HGTypes.h"
#include "HexGrid/InHexActorInterface.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "AITask_AutoChessMoveTo.generated.h"

class UAutoChessAIBBContext;
class ATheOneAutoChessAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTheOneMoveToSuccessSignature, bool, TargetIsGoingToBooked, FHCubeCoord, TargetBookedCoord);
/**
 * 移动到可以释放技能的位置，当可以释放技能时，返回成功； 其它情况返回失败
 * UAITask 不会因为BTTask的终止而终止， 需要手动EndTask来销毁
 */
UCLASS()
class THEONE_API UAITask_AutoChessMoveTo : public UAITask
{
	GENERATED_BODY()

public:
	UAITask_AutoChessMoveTo(const FObjectInitializer& ObjectInitializer);
	
	//创建Task对象-静态
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "TRUE", DisplayName = "AutoChess Move To Release Ability HexGrid"))
	static UAITask_AutoChessMoveTo* AutoChessMoveTo(AAIController* InSourceAI,
													const FBlackboardKeySelector& InTargetActorBlackboardKey,
													const FBlackboardKeySelector& InTargetLocationBlackboardKey,
													const FBlackboardKeySelector& InGoalDistanceBlackboardKey,
													float InAcceptanceRadius,
													EAIOptionFlag::Type InAcceptPartialPath = EAIOptionFlag::Enable,
													EAIOptionFlag::Type InRequireNavigableEndLocation = EAIOptionFlag::Enable,
													EAIOptionFlag::Type InProjectGoalOnNavigation = EAIOptionFlag::Enable,
													bool InAllowStrafe = false,
													bool InReachTestIncludesAgentRadius = false,
													bool InReachTestIncludesGoalRadius = true,
													bool InbLockAILogic = false);
	
protected:
	EBlackboardNotificationResult OnTargetActorValueChange(const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key);
	EBlackboardNotificationResult OnTargetLocationValueChange(const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key);
	EBlackboardNotificationResult OnGoalDistanceValueChange(const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key);
	//触发任务函数
	virtual void Activate() override;
	virtual void OnDestroy(bool InbOwnerFinished) override;

public:

	UPROPERTY()
	TWeakObjectPtr<ATheOneAutoChessAIController> WeakOwnerHexController;

	UPROPERTY()
	TScriptInterface<IInHexActorInterface> InHexActorInterface;

	int32 TargetActorBlackboardKeyID;
	FName TargetActorBlackboardKeyName;
	int32 TargetLocationBlackboardKeyID;
	FName TargetLocationBlackboardKeyName;
	int32 GoalDistanceBlackboardKeyID;
	FName GoalDistanceBlackboardKeyName;
	FDelegateHandle LocationBBKeyChangeHandle;
	FDelegateHandle ActorBBKeyChangeHandle;
	FDelegateHandle GoalDistanceBBKeyChangeHandle;
	
	//目标角色
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;
	
	UPROPERTY()
	FHCubeCoord GoalCoord;


protected:
	/** parameters of move request */
	UPROPERTY()
	FAIMoveRequest MoveRequest;
	
	/** request ID of path following's request */
	FAIRequestID MoveRequestID;

	/** handle of path following's OnMoveFinished delegate */
	FDelegateHandle PathFinishDelegateHandle;

	//////////// MoveRequest Parameters Start ////////////
	// 目标距离
	UPROPERTY()
	int GoalDistance;
	
	//////////// MoveRequest Parameters End ////////////

	//** handle of active ConditionalPerformMove timer  */
	FTimerHandle MoveRetryTimerHandle;
	
	//委托-成功结束
	UPROPERTY(BlueprintAssignable)
	FTheOneMoveToSuccessSignature OnRequestSuccess;

	//委托-失败结束
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnRequestFailed;
	
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnAbort;

	void ConditionalPerformMove();

	void OnRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);
	//执行移动函数
	void PerformMove();

	//成功完成任务函数
	UFUNCTION()
	void FinishMoveTask();

	UFUNCTION()
	void RequestSuccess(bool TargetIsGoingToBooked,  const FHCubeCoord& InTargetBookedCoord);

	UFUNCTION()
	void RequestFailed();

	// Abort task Start
public:
	UFUNCTION(BlueprintCallable)
	void AbortTask();

	/**
	 * 仅仅结束任务， 角色仍会继续前往最后一次寻路设置的位置
	 */
	UFUNCTION(BlueprintCallable)
	void SimpleEndTask();
private:
	void RequestAbort();
	
	bool bIsAborting = false;
	// Abort task End
};
