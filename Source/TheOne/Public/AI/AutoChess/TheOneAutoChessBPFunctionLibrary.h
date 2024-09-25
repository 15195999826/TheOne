// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutoChessAIBBContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TheOneAutoChessBPFunctionLibrary.generated.h"

class AAIController;
class UAutoChessAIBBContext;
enum class ETheOneValid : uint8;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAutoChessBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs = "IsValid"))
	static void SelectAbilityAndTarget(AAIController* SelfAI, UAutoChessAIBBContext* InContext, ETheOneValid& IsValid);

	UFUNCTION(BlueprintCallable)
	static float RotateToActor(AActor* SourceActor, AActor* TargetActor, float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	static float RotateToPoint(AActor* SourceActor, const FVector& TargetPoint, float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	UAutoChessAIBBContext* GetAIBBContext(AAIController* InController);

	/**
	 * @brief 检查是否有Actor站在格子上, 未考虑格子被订阅以及棋子正在移动的情况
	 * @param WorldContextObject
	 * @param InLocation
	 * @param IsValid
	 * @return
	 */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", ExpandEnumAsExecs = "IsValid"))
	static AActor* HasActorStandingOnGrid(const UObject* WorldContextObject, const FVector& InLocation, ETheOneValid& IsValid);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static bool GetRandomEmptyLocationOnHexGrid(const UObject* WorldContextObject, FVector& OutLocation, const FVector& InLocation, int32 Radius);
};
