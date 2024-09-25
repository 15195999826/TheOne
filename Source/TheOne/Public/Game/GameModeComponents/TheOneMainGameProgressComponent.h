// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TheOneMainGameProgressComponent.generated.h"

UENUM(BlueprintType)
enum class ETheOneGameProgress : uint8
{
	None,
	Town,
	Task,
	Battle
};



UCLASS(Blueprintable, ClassGroup=(TheOneGameModeComponent), meta=(BlueprintSpawnableComponent) )
class THEONE_API UTheOneMainGameProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTheOneMainGameProgressComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
		
};
