// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnLua.h"
#include "TheOneLuaEnvSystem.generated.h"

/**
 * C++ Call Lua 只能调用全局函数，全局函数中，暂时不知道如何传递self,勉强可以通过全局函数调用M:Function的方式实现读到Self 
 */
UCLASS()
class THEONE_API UTheOneLuaEnvSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UnLua::FLuaEnv Env;

	static void PrintScreen(const FString& Msg);

	void TestCallLuaByGlobalTable();
	void TestCallLuaByFLuaTable();
};
