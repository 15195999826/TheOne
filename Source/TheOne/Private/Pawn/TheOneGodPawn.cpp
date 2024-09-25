// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/TheOneGodPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Game/TheOnePlayerControllerBase.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATheOneGodPawn::ATheOneGodPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SceneRoot);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	CursorDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("CursorDecal"));
	CursorDecal->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void ATheOneGodPawn::BeginPlay()
{
	Super::BeginPlay();
	DefaultRotation = CameraBoom->GetComponentRotation();
	DefaultZoom = CameraBoom->TargetArmLength;
	CurrentZoom = DefaultZoom;
	OnWindowResized();
	// 监听WindowResized事件
	FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &ATheOneGodPawn::OnWindowResized);
	
	auto PC = Cast<ATheOnePlayerControllerBase>(GetOwner());

	// 如果是战斗关卡的玩家控制器
	if (PC && PC->IsLocalPlayerController())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(BattleInputMappingContext, 0);
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("[ATheOneGodPawn::SetupPlayerInputComponent] Subsystem is null"));
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EnhancedInputComponent->BindAction(RotateCameraAxisAction, ETriggerEvent::Triggered, this,
											   &ATheOneGodPawn::OnRotateCamera);
			EnhancedInputComponent->BindAction(ResetCameraAction, ETriggerEvent::Started, this,
											   &ATheOneGodPawn::OnResetCamera);
			EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &ATheOneGodPawn::OnZoomCamera);
			EnhancedInputComponent->BindAction(MoveCameraAxisAction, ETriggerEvent::Triggered, this, &ATheOneGodPawn::OnMoveCamera);
			EnhancedInputComponent->BindAction(LockToPlayerAction, ETriggerEvent::Started, this, &ATheOneGodPawn::OnLockToGodStart);
			EnhancedInputComponent->BindAction(LockToPlayerAction, ETriggerEvent::Completed, this, &ATheOneGodPawn::OnLockToGodEnd);
			
			// InputComponent->BindAction<FFooDelegate>("Tes1t", IE_Pressed, this, &ATheOneGodPawn::TestFunc, 42);
			
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("[ATheOneGodPawn::SetupPlayerInputComponent] EnhancedInputComponent is null"));
		}
	}
}

void ATheOneGodPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(!FollowPawn)
	{
		SetActorLocation(FMath::Lerp(CameraBoom->GetComponentLocation(), DesiredLocation, DeltaSeconds * TraceSpeed));
		// Lerp the camera to the desired position
		ApplyEdgeScrolling();
	} else
	{
		// Set camera to follow pawn
		SetActorLocation(GetActorLocation());
		DesiredLocation = GetActorLocation();
	}
	
	// zoom
	if (FMath::IsNearlyEqual(CurrentZoom, DesiredZoom))
	{
		return;
	}
	
	CurrentZoom = FMath::Lerp(CurrentZoom, DesiredZoom, DeltaSeconds * ZoomSpeed);
	CameraBoom->TargetArmLength = CurrentZoom;
}

void ATheOneGodPawn::OnMoveCamera(const FInputActionValue& InputActionValue)
{
	auto InputVector = InputActionValue.Get<FVector2D>();
	// Get the current yaw rotation of the camera boom
	FRotator CameraRotation = CameraBoom->GetComponentRotation();
	FRotator YawRotation(0, CameraRotation.Yaw, 0);

	// Calculate forward and right vectors
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Calculate the desired movement direction
	FVector DesiredMovementDirection = (ForwardVector * InputVector.Y + RightVector * InputVector.X).GetSafeNormal();

	// Update the desired location based on the movement direction
	DesiredLocation += DesiredMovementDirection * MoveSpeed;

}

void ATheOneGodPawn::OnZoomCamera(const FInputActionValue& Value)
{
	DesiredZoom = FMath::Clamp(
		CurrentZoom + Value.GetMagnitude() * -ZoomSpeed,
		MinimumZoom,
		MaximumZoom
	);
}

void ATheOneGodPawn::OnRotateCamera(const FInputActionValue& Value)
{
	FRotator NewRotation = CameraBoom->GetComponentRotation();
	NewRotation.Yaw += Value.GetMagnitude() * RotateSpeed;
	CameraBoom->SetWorldRotation(NewRotation);
}

void ATheOneGodPawn::OnResetCamera(const FInputActionValue& InputActionValue)
{
	CameraBoom->SetWorldRotation(DefaultRotation);
	// Reset the camera zoom
	DesiredZoom = DefaultZoom;
}

void ATheOneGodPawn::OnLockToGodStart(const FInputActionValue& Value)
{
	FollowPawn = true;
}

void ATheOneGodPawn::OnLockToGodEnd(const FInputActionValue& Value)
{
	FollowPawn = false;
}

void ATheOneGodPawn::ApplyEdgeScrolling()
{
	if (!EnableEdgeScrolling)
	{
		return;
	}
	
	float DirectionX = 0;
	float DirectionY = 0;
	
	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	if (this->IsValidMousePosition(MousePosition, Top))
	{
		DirectionX = 1;
	}
	else if (IsValidMousePosition(MousePosition, Bottom))
	{
		DirectionX = -1;
	}
	
	if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Right))
	{
		DirectionY = 1;
	}
	else if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Left))
	{
		DirectionY = -1;
	}

	DesiredLocation = CameraBoom->GetComponentLocation() +  FVector(  DirectionX * MoveSpeed, DirectionY * MoveSpeed, 0.0f);
}

bool ATheOneGodPawn::IsValidMousePosition(const FVector2D Positions, const FVector4& Rules)
{
	const bool InputY = Positions.Y >= Rules.X && Positions.Y <= Rules.Y;
	const bool InputX = Positions.X >= Rules.Z && Positions.X <= Rules.W;
	return InputX && InputY;
}

void ATheOneGodPawn::OnWindowResized()
{
	const FVector2D Result = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	Top = FVector4(0, 0, 0, 0);
	Bottom = FVector4(0, 0, 0, 0);
	Left = FVector4(0, 0, 0, 0);
	Right = FVector4(0, 0, 0, 0);

	Top.Y = (MovementZoneInPercent / 100.f) * Result.Y;
	Top.W = Result.X;

	Bottom.X = (1 - (MovementZoneInPercent / 100.f)) * Result.Y;
	Bottom.Y = Result.Y;
	Bottom.W = Result.X;

	Left.Y = (MovementZoneInPercent / 100.f) * Result.X;
	Left.W = Result.Y;

	Right.X = (1 - (MovementZoneInPercent / 100.f)) * Result.X;
	Right.Y = Result.X;
	Right.W = Result.Y;	
}