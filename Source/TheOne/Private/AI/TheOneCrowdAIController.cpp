// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TheOneCrowdAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

ATheOneCrowdAIController::ATheOneCrowdAIController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}
