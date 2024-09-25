// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"
#include "AITask_WaitTargetArrived.generated.h"

class ATheOneAutoChessAIController;
class UAutoChessAIBBContext;
/**
 * 
 */
UCLASS()
class THEONE_API UAITask_WaitTargetArrived : public UAITask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "TRUE", DisplayName = "AutoChess Wait Target Arrived"))
	static UAITask_WaitTargetArrived* WaitTargetArrived(AAIController* InSourceAI, UAutoChessAIBBContext* InContext);

protected:
	//触发任务函数
	virtual void Activate() override;

public:
	UPROPERTY()
	TWeakObjectPtr<ATheOneAutoChessAIController> SourceAI;
	
	//目标角色
	UPROPERTY()
	TWeakObjectPtr<UAutoChessAIBBContext> AIContext;

protected:
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate NoNeedToWait;
	
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnTargetArrived;
	
	//委托-失败结束
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnRequestFailed;
	
	FDelegateHandle SegmentFinishedDelegate;
	
protected:
	void OnAIArrived();

	UFUNCTION()
	void RequestNoNeedToWait();

	UFUNCTION()
	void RequestOnTargetArrived();

	UFUNCTION()
	void RequestFailed();
};
