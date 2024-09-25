// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TheOneRTSAIController.h"

#include "Character/TheOnePathFollowingComponent.h"

ATheOneRTSAIController::ATheOneRTSAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTheOnePathFollowingComponent>(TEXT("PathFollowingComponent")))
	// : Super(ObjectInitializer)
{
	
}
