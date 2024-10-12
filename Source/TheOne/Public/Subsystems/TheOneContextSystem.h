// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TheOneContextSystem.generated.h"

class ATheOneBattleEvaluate;
class ATheOneBattle;
class ATheOneCharacterBase;
class AHexGrid;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayEventTriggerOverSignature);
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneContextSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ATheOneLevelSettingActor> LevelSetting;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ATheOneBattle> Battle;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ATheOneBattleEvaluate> BattleEvaluate;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AHexGrid> HexGrid;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakSelectedCharacter;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakBagSelectCharacter;

	// 当有角色触发了被动技能时，监听是否所有人的反应事件已经结束
	UPROPERTY(BlueprintAssignable)
	FOnGameplayEventTriggerOverSignature OnGameplayEventTriggerOver;

	void RegisterGameplayEventWaiting(AActor* Actor);
	void UnRegisterGameplayEventWaiting(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasGameplayEventWaiting() const { return GameplayEventWaitingActors.Num() > 0; }
	
	UFUNCTION(BlueprintCallable)
	void DebugGameplayEventWaitingActors();
	
private:
	UPROPERTY()
	TArray<AActor*> GameplayEventWaitingActors;
};

