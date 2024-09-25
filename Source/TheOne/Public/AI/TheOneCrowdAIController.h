// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/TheOneAIController.h"
#include "TheOneCrowdAIController.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API ATheOneCrowdAIController : public ATheOneAIController
{
	GENERATED_BODY()

public:
	ATheOneCrowdAIController(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());
	
};
