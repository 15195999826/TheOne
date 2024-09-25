// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TheOneTutorialComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(TheOneGameModeComponent), meta=(BlueprintSpawnableComponent) )
class THEONE_API UTheOneTutorialComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTheOneTutorialComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
