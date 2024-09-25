// Fill out your copyright notice in the Description page of Project Settings.


#include "TheOneLuaBlueprintFunctionLibrary.h"

#include "UnLua.h"
#include "Kismet/KismetSystemLibrary.h"

static void PrintScreen(const FString& Msg)
{
    UKismetSystemLibrary::PrintString(nullptr, Msg, true, false, FLinearColor(0, 0.66, 1), 100);
}

void UTheOneLuaBlueprintFunctionLibrary::CallLuaByGlobalTable()
{
    

    UnLua::FLuaEnv Env;
    
}

void UTheOneLuaBlueprintFunctionLibrary::CallLuaByFLuaTable()
{
    
    UnLua::FLuaEnv Env;

   
}

bool CustomLoader1(UnLua::FLuaEnv& Env, const FString& RelativePath, TArray<uint8>& Data, FString& FullPath)
{
    const auto SlashedRelativePath = RelativePath.Replace(TEXT("."), TEXT("/"));
    FullPath = FString::Printf(TEXT("%s%s.lua"), *GLuaSrcFullPath, *SlashedRelativePath);

    if (FFileHelper::LoadFileToArray(Data, *FullPath, FILEREAD_Silent))
        return true;

    FullPath.ReplaceInline(TEXT(".lua"), TEXT("/Index.lua"));
    if (FFileHelper::LoadFileToArray(Data, *FullPath, FILEREAD_Silent))
        return true;

    return false;
}

bool CustomLoader2(UnLua::FLuaEnv& Env, const FString& RelativePath, TArray<uint8>& Data, FString& FullPath)
{
    const auto SlashedRelativePath = RelativePath.Replace(TEXT("."), TEXT("/"));
    const auto L = Env.GetMainState();
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    const char* Path = lua_tostring(L, -1);
    lua_pop(L, 2);
    if (!Path)
        return false;

    TArray<FString> Parts;
    FString(Path).ParseIntoArray(Parts, TEXT(";"), false);
    for (auto& Part : Parts)
    {
        Part.ReplaceInline(TEXT("?"), *SlashedRelativePath);
        FPaths::CollapseRelativeDirectories(Part);
        
        if (FPaths::IsRelative(Part))
            FullPath = FPaths::ConvertRelativePathToFull(GLuaSrcFullPath, Part);
        else
            FullPath = Part;

        if (FFileHelper::LoadFileToArray(Data, *FullPath, FILEREAD_Silent))
            return true;
    }

    return false;
}

void UTheOneLuaBlueprintFunctionLibrary::SetupCustomLoader(int Index)
{
    switch (Index)
    {
    case 0:
        FUnLuaDelegates::CustomLoadLuaFile.Unbind();
        break;
    case 1:
        FUnLuaDelegates::CustomLoadLuaFile.BindStatic(CustomLoader1);
        break;
    case 2:
        FUnLuaDelegates::CustomLoadLuaFile.BindStatic(CustomLoader2);
        break;
    }
}
