// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "TheOneCheatManager.generated.h"

enum class ECheatFunction : uint8;
DECLARE_LOG_CATEGORY_EXTERN(LogTheOneCheat, Log, All);
/**
 * 
 */
UCLASS(config = Game, Within = PlayerController, MinimalAPI)
class UTheOneCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	virtual void InitCheatManager() override;

	UFUNCTION(Exec)
	void CheatAddOneAI();

	UFUNCTION(Exec)
	void TestCheat();

	UFUNCTION(Exec)
	void TestCheatParam(const FString& Msg);

	UFUNCTION(BlueprintCallable)
	bool IsEnableCheat() const { return bEnableCheat; }

	UFUNCTION(Exec)
	void ToggleDebugHexPath();

private:
	bool bEnableCheat = false;
	void OnLevelPrepared();
	void CheatExecute(ECheatFunction Function, const FString& Msg = TEXT(""));
};
