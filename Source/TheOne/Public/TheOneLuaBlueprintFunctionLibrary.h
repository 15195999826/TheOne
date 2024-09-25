// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TheOneLuaBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneLuaBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CallLuaByGlobalTable", Category = "UnLua"))
	static void CallLuaByGlobalTable();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CallLuaByFLuaTable", Category = "UnLua"))
	static void CallLuaByFLuaTable();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetupCustomLoader", Category = "UnLua"))
	static void SetupCustomLoader(int Index);
};
