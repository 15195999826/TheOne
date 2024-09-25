// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess//TheOneAutoChessAIController.h"

#include "AI/AutoChess/AutoChessAIBBContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HexGrid/HexPathFollowingComponent.h"

/**
 * Here we assign our HGPathFollowingComponent class to the HGAIController so when we make an instance of this class it will use our component instead of the default,
 * keep attention on the TEXT() parameter, to work well it must be the same as the default component of the parent AIController class!
 */
ATheOneAutoChessAIController::ATheOneAutoChessAIController(const FObjectInitializer &ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHexPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
	AIBBContext = CreateDefaultSubobject<UAutoChessAIBBContext>(TEXT("AIBBContext"));
}

bool ATheOneAutoChessAIController::IsDead() const
{
	return GetBlackboardComponent()->GetValueAsBool("IsDead");
}