// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneGameplayAbility.h"

#include "TheOneDataDrivePassiveGA.generated.h"
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
	int32 RegisterBattleEvent(const FTheOneAbilityConfig* AbilityRow);

	void RemoveBattleEvent(int32 InEventID);
	void SetupDataDrivePassiveAbility();

	AActor* GetCurrentEventTriggerActor() const;
	
protected:
	int32 SelfEventID = 0;
	FGameplayEventData CurrentEventPayload;
	
	TMap<FGameplayTag, TArray<int32>> BattleEvents;

	// 一个EventID就对应了一个被动技能，Event2MontageActionPointMap和Event2ActionMap都可以存在它的功能
	// 带有动画的被动技能， 动画部分事件的功能执行
	TMap<int32, TArray<FTheOneAbilityActionPoint>> EventID2MontageActionPointsMap;

	// 动画无关的被动技能效果

	TMap<int32, FTheOneAbilityActionContainer> EventID2ActionMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<int32, TObjectPtr<UAnimMontage>> EventID2MontageMap;
	// 检测CD，检测次数限制
	UFUNCTION(BlueprintCallable)
	TArray<int32> ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);
	UFUNCTION(BlueprintCallable)
	void ExecuteMontageAction(int32 InEventID, FGameplayTag InActionPoint);
	UFUNCTION(BlueprintCallable)
	void ExecuteOver();
	
};
