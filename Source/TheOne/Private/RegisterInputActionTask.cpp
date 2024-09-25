// Fill out your copyright notice in the Description page of Project Settings.


#include "RegisterInputActionTask.h"

#include "Game/TheOnePlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"

URegisterInputActionTask::~URegisterInputActionTask()
{
	UE_LOG(LogTemp, Log, TEXT("URegisterInputActionTask::~URegisterInputActionTask"));
}

URegisterInputActionTask* URegisterInputActionTask::RegisterInputActionTask(const UObject* WorldContextObject, FKey InKey, TEnumAsByte<EInputEvent> InInputEvent, URegisterInputActionTask*& OutTask)
{
	GlobalTaskID++;
	URegisterInputActionTask* BlueprintNode = NewObject<URegisterInputActionTask>();
	// Add to viewport
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TaskID = GlobalTaskID;
	BlueprintNode->Key = InKey;
	BlueprintNode->InputEvent = InInputEvent;
	// Return
	OutTask = BlueprintNode;
	return BlueprintNode;
}

void URegisterInputActionTask::Activate()
{
	Super::Activate();

	auto TheOnePC = Cast<ATheOnePlayerControllerBase>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	if (TheOnePC)
	{
		Delegate.BindUObject(this, &URegisterInputActionTask::OnInputAction);
		TheOnePC->RegisterInputAction(this);
	}
	else
	{
		SetReadyToDestroy();
	}
}

void URegisterInputActionTask::EndTask()
{
	SetReadyToDestroy();
	// Todo: 主动调用GC会销毁这个Task
	// GEngine->ForceGarbageCollection(true);
}

void URegisterInputActionTask::ActiveEnd()
{
	auto TheOnePC = Cast<ATheOnePlayerControllerBase>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	if (TheOnePC)
	{
		TheOnePC->UnregisterInputAction(TaskID);
	}
	FTimerHandle UnusedHandle;
	// 下一帧销毁
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &URegisterInputActionTask::EndTask, 0.01f, false);
}

void URegisterInputActionTask::OnInputAction()
{
	OnExecute.Broadcast();
}

class UWorld* URegisterInputActionTask::GetWorld() const
{
	return WorldContextObject->GetWorld();
}
