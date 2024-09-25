// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TheOneCharacterMovementComponent.h"

float UTheOneCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_NavWalking || MovementMode ==  MOVE_Walking)
	{
		auto RotationRateYaw = FMath::Abs(RotationRate.Yaw);
		// 输入RotationRateYaw进入一个分段函数， 当大于360时， 范围最大移速
		// 当小于360时，根据当前旋转速度，计算出角色朝向与移动方向的夹角小于多少时， 才能范围最大移速
		if (RotationRateYaw >= 360.0f)
		{
			return Super::GetMaxSpeed();
		}
		auto CurrentRotation = UpdatedComponent->GetComponentRotation();
		auto DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation,0, NoUseFilter);
		auto DeltaAngle = FMath::Abs(FRotator::NormalizeAxis(DesiredRotation.Yaw - CurrentRotation.Yaw));
		
		auto MinCanMoveAngle = RotationBaseLimit + RotationRateYaw * RotationLimitCoe;
		if (DeltaAngle > MinCanMoveAngle)
		{
			return 0.f;
		}
	}

	return Super::GetMaxSpeed();
}