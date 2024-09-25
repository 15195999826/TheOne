// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/TheOneAIController.h"
#include "HexGrid/InHexActorInterface.h"
#include "TheOneAutoChessAIController.generated.h"

class UAutoChessAIBBContext;
class UHexPathFollowingComponent;
struct FHCubeCoord;
/**
 * This is the parent class of BP_AIController_Example_B
 *
 * We need this inherited class just only to set and use our custom HGPathFollowingComponent
 * (look at the constructor in .cpp)
 *
 */
UCLASS()
class THEONE_API ATheOneAutoChessAIController : public ATheOneAIController
{
	GENERATED_BODY()
	
public:
	ATheOneAutoChessAIController(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAutoChessAIBBContext> AIBBContext;

	bool IsDead() const;
};
