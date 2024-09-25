// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "TheOneInteractActor.generated.h"

class UWidgetComponent;
class UArrowComponent;

UENUM(BlueprintType)
enum class ETheOneInteractType : uint8
{
	None,
	Battle UMETA(DisplayName="战斗"),
	BPCustom UMETA(DisplayName="蓝图自定义"),
};

USTRUCT(BlueprintType)
struct FTheOneInteractAbility
{
	GENERATED_BODY()

	FTheOneInteractAbility()
	{
		Type = ETheOneInteractType::None;
	}

	UPROPERTY(EditAnywhere)
	ETheOneInteractType Type;

	// Todo:
};

UCLASS()
class THEONE_API ATheOneInteractActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheOneInteractActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category=Default)
	FTransform SpawnOffset;

protected:
	UPROPERTY(EditDefaultsOnly, Category=Default)
	FTheOneInteractAbility Ability;
	
	UPROPERTY(Category=InteractActor, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> ModelRoot;

	UPROPERTY(Category=InteractActor, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> TipUI;

#if WITH_EDITORONLY_DATA
	/** Component shown in the editor only to indicate character facing */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> EditorFloor;
#endif

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnFocus"))
	void BP_OnFocus();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnLoseFocus"))
	void BP_OnLoseFocus();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnLeftMouseButtonClick"))
	void BP_OnLeftMouseButtonClick();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnRightMouseButtonClick"))
	void BP_OnRightMouseButtonClick();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnInteract"))
	void BP_OnInteract();

private:
	UFUNCTION(BlueprintCallable)
	void OnInteractBase();
	
	UFUNCTION(BlueprintCallable)
	void OnFocus();

	UFUNCTION(BlueprintCallable)
	void OnLoseFocus();

	UFUNCTION(BlueprintCallable)
	void OnLeftMouseButtonClick();

	UFUNCTION(BlueprintCallable)
	void OnRightMouseButtonClick();
};
