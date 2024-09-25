// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TheOneLuaEnvSystem.h"

#include "Kismet/KismetSystemLibrary.h"

void UTheOneLuaEnvSystem::PrintScreen(const FString& Msg)
{
	UKismetSystemLibrary::PrintString(nullptr, Msg, true, false, FLinearColor(0, 0.66, 1), 100);
}

void UTheOneLuaEnvSystem::TestCallLuaByGlobalTable()
{
	PrintScreen(TEXT("[C++]CallLuaByGlobalTable 开始"));
	// 文件夹使用.分隔，如 Tutorials.CPPCallLua
	const auto bSuccess = Env.DoString("G_CppCallLua = require 'CPPCallLua'");
	check(bSuccess);

	const auto RetValues = UnLua::CallTableFunc(Env.GetMainState(), "G_CppCallLua", "SayHi", "C++ Call Lua");
	check(RetValues.Num() == 1);

	const auto Msg = FString::Printf(TEXT("[C++]Say Hi Return，结果=%f"), RetValues[0].Value<float>());
	PrintScreen(Msg);

	const auto RetValues2 = UnLua::CallTableFunc(Env.GetMainState(), "G_CppCallLua", "OtherSayHi", "C++ Call Lua2");
	check(RetValues2.Num() == 1);

	const auto Msg2 = FString::Printf(TEXT("[C++]OtherSayHi return，结果=%f"), RetValues2[0].Value<float>());
	PrintScreen(Msg2);
	
	PrintScreen(TEXT("[C++]CallLuaByGlobalTable 结束"));
}

void UTheOneLuaEnvSystem::TestCallLuaByFLuaTable()
{
	PrintScreen(TEXT("[C++]CallLuaByFLuaTable 开始"));
	const auto Require = UnLua::FLuaFunction(&Env, "_G", "require");
	const auto RetValues1 = Require.Call("CPPCallLua");
	check(RetValues1.Num() == 2);

	const auto RetValue = RetValues1[0];
	const auto LuaTable = UnLua::FLuaTable(&Env, RetValue);
	const auto RetValues2 = LuaTable.Call("SayHi", "C++ Call Lua");
	check(RetValues2.Num() == 1);

	const auto Msg = FString::Printf(TEXT("[C++]收到来自Lua的返回，结果=%f"), RetValues2[0].Value<float>());
	PrintScreen(Msg);
	
	PrintScreen(TEXT("[C++]CallLuaByFLuaTable 结束"));
}
