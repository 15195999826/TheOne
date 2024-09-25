// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Game/CursorTraceInterface.h"
#include "GameFramework/Pawn.h"
#include "TheOneGodPawn.generated.h"
DECLARE_DELEGATE_OneParam(FFooDelegate, int32);

/**
 * 1、 通过添加或移除MappingContext来控制选中单位和未选中单位时的操作
 */
UCLASS()
class THEONE_API ATheOneGodPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATheOneGodPawn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
// Input Start	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	// 相机伸缩臂
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;
	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Cursor)
	TObjectPtr<UDecalComponent> CursorDecal;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> BattleInputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TheOne Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> RotateCameraAxisAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TheOne Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> ResetCameraAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TheOne Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> MoveCameraAxisAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TheOne Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> ZoomCameraAction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TheOne Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> LockToPlayerAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float DesiredZoom = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float ZoomSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float MinimumZoom = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float MaximumZoom = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float MoveSpeed = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float TraceSpeed = 1.50f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	float RotateSpeed = 1.0f;

	// 是否跟随Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	bool FollowPawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	FVector DesiredLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TheOne Enhanced Camera | Movement Settings")
	bool EnableEdgeScrolling = true;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "TheOne Enhanced Camera | Movement Settings",
		meta=(EditCondition="EnableEdgeScrolling")
		)
	float MovementZoneInPercent = 5;

private:
	float CurrentZoom;
	FVector4 Top = FVector4();
	FVector4 Bottom = FVector4();
	FVector4 Left = FVector4();
	FVector4 Right = FVector4();

	bool bInputSetup = false;
	FRotator DefaultRotation;
	float DefaultZoom;
private:
	void OnMoveCamera(const FInputActionValue& InputActionValue);
	void OnZoomCamera(const FInputActionValue& Value);
	void OnRotateCamera(const FInputActionValue& Value);
	void OnResetCamera(const FInputActionValue& InputActionValue);
	void OnLockToGodStart(const FInputActionValue& Value);
	void OnLockToGodEnd(const FInputActionValue& Value);
	void ApplyEdgeScrolling();
	bool IsValidMousePosition(FVector2D Positions, const FVector4& Rules);
	void OnWindowResized();

	void TestFunc(int32 Test);
//Input End
};
