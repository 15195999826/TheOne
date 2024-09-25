// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheOneAICommandInterface.h"
#include "Types/TheOneCharacterConfigStruct.h"
#include "TheOneAIController.generated.h"
class ATheOneCharacterBase;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class THEONE_API ATheOneAIController : public AAIController, public ITheOneAICommandInterface
{
	GENERATED_BODY()

public:
	ATheOneAIController(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakTheOneCharacterBase;

	void SetPawnSpawnInfo(const FTheOneAIPawnSpawnInfo& InPawnSpawnInfo) { PawnSpawnInfo = InPawnSpawnInfo; }

	UFUNCTION(BlueprintCallable)
 	const FTheOneAIPawnSpawnInfo& GetPawnSpawnInfo() const { return PawnSpawnInfo; }

	UFUNCTION(BlueprintImplementableEvent)
	void StartAI();
	
protected:
	UPROPERTY()
	FTheOneAIPawnSpawnInfo PawnSpawnInfo;
	
};
