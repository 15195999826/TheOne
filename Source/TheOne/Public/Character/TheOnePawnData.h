// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TheOnePawnData.generated.h"

/**
 * 暂时没想到这个有什么用
 * 也许， 同一个BP的Character， 但是拥有不同的技能、属性等， 可以给每一个角色做一个这个
 */
UCLASS()
class THEONE_API UTheOnePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
};
