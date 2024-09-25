// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/TheOneAIController.h"
#include "TheOneRTSAIController.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API ATheOneRTSAIController : public ATheOneAIController
{
	GENERATED_BODY()

public:
	ATheOneRTSAIController(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());
};
