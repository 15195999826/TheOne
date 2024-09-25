// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneGameplayAbility.h"

#include "TheOneDataDrivePassiveGA.generated.h"

USTRUCT()
struct FTheOneDataDriveBattleEvent
{
	GENERATED_BODY()

	UPROPERTY()
	int32 EventID = -1;

	UPROPERTY()
	FName AbilityConfigName;
};

/**
 * 核心思路:
 * 1、角色创建就立刻激活当前被动技能，并且永远不会关闭
 * 2、向外部提供RegisterBattleEvent接口，当激活数据驱动被动技能时，注册时间，关闭时移除注册
 */
UCLASS()
class THEONE_API UTheOneDataDrivePassiveGA : public UTheOneGameplayAbility
{
	GENERATED_BODY()

public:
	int32 RegisterBattleEvent(FGameplayTag InBattleEvent, FName InAbilityConfigName);

	void RemoveBattleEvent(int32 InEventID);
	void SetupDataDrivePassiveAbility();

protected:
	int SelfEventID = 0;
	
	TMap<FGameplayTag, TArray<FTheOneDataDriveBattleEvent>> BattleEvent2AbilityConfigMap;

	// 检测CD，检测次数限制
	UFUNCTION(BlueprintCallable)
	void ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);
};
