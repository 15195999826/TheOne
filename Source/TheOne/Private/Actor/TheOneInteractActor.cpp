// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/TheOneInteractActor.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
ATheOneInteractActor::ATheOneInteractActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->CanCharacterStepUpOn = ECB_Yes;
	SphereComponent->SetShouldUpdatePhysicsVolume(false);
	SphereComponent->bDynamicObstacle = false;
	RootComponent = SphereComponent;

	ModelRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ModelRoot"));
	if (ModelRoot)
	{
		ModelRoot->SetupAttachment(SphereComponent);
	}
	
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetRelativeRotation(FRotator(0, 180, 0));
		ArrowComponent->SetupAttachment(SphereComponent);
		ArrowComponent->bIsScreenSizeScaled = true;
		ArrowComponent->SetSimulatePhysics(false);
	}

	EditorFloor = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("EditorFloor"));
	if (EditorFloor)
	{
		EditorFloor->SetupAttachment(SphereComponent);
		EditorFloor->SetCollisionProfileName(FName(TEXT("NoCollision")));
		EditorFloor->SetGenerateOverlapEvents(false);
		EditorFloor->SetCanEverAffectNavigation(false);
	}
#endif

	TipUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("TipUI"));
	TipUI->SetupAttachment(SphereComponent);
}

// Called when the game starts or when spawned
void ATheOneInteractActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATheOneInteractActor::OnInteractBase()
{
	switch (Ability.Type)
	{
		case ETheOneInteractType::BPCustom:
			BP_OnInteract();
			break;
	}
	
}

void ATheOneInteractActor::OnFocus()
{
	BP_OnFocus();
}

void ATheOneInteractActor::OnLoseFocus()
{
	BP_OnLoseFocus();
}

void ATheOneInteractActor::OnLeftMouseButtonClick()
{
	BP_OnLeftMouseButtonClick();
}

void ATheOneInteractActor::OnRightMouseButtonClick()
{
	BP_OnRightMouseButtonClick();
}

