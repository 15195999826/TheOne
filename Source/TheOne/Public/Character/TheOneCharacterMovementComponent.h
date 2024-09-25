// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheOneCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	inline static FRotator NoUseFilter{};
	/**
	 * 当转身速率小于RotateSpeedAffectSpeed时生效
	 * 当移动方向与角色朝向的夹角小于该值时，角色可以行走;
	 */
	UPROPERTY(EditAnywhere)
	float RotationLimitCoe = 0.05f;
	UPROPERTY(EditAnywhere)
	float RotationBaseLimit = 25.0f;
	
	virtual float GetMaxSpeed() const override;
};
