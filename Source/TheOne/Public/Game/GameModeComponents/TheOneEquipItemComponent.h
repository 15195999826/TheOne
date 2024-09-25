// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/TheOneItemSystem.h"
#include "TheOneEquipItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEONE_API UTheOneEquipItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTheOneEquipItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

private:
	void OnItemCreated(const FTheOneItemInstance& InItemInstance);
	void OnItemDeleted(const FTheOneItemInstance& InItemInstance);
	void OnItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance);
};
