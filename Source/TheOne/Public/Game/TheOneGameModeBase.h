// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TheOneCharacterBase.h"
#include "GameFramework/GameModeBase.h"
#include "TheOneGameModeBase.generated.h"

class AAIController;
class UTheOneAutoChessBattleComponent;
class ATheOneInteractActor;
struct FTheOneAIPawnSpawnInfo;
class ATheOneAIController;
/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTheOneGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);

/**
 * GameMode是游戏规则
 */
UCLASS()
class THEONE_API ATheOneGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UTheOneAutoChessBattleComponent* GetAutoChessBattleComponent() const;
	/**
	 * 通用玩家初始化， 目前只手动调用在生成AI， 总是先调用这个函数， 再调用RestartPlayer， AIController初始化完成， 但是Pawn还未生成
	 */
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	
	/**
	 * 获取Controller的默认Pawn类
	 * @param InController 
	 * @return 
	 */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	// 通过这两个函数的重写， 玩家的出生位置永远不会在InitNewPlayer中指定， 永远通过FindPlayerStart_Implementation、ChoosePlayerStart_Implementation来获取
	// 并且由于ShouldSpawnAtStartSpot设置为False， 在执行FindPlayerStart函数， 只要IncomingName为空， 就一定会执行ChoosePlayerStart
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;

	/**
	 * 选择玩家的出生位置
	 * @param Player 
	 * @return 
	 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	virtual void PostInitializeComponents() override;

public:
	FSimpleMulticastDelegate OnLevelPrepared;
	/**
	 * AIController初始化完成， 但是Pawn还未生成
	 */
	FOnTheOneGameModePlayerInitialized OnTheOneGameModePlayerInitialized;

	UPROPERTY(EditAnywhere)
	TMap<uint32, TObjectPtr<ATheOneAIController>> SpawnedAIMap;

protected:
	UPROPERTY()
	TWeakObjectPtr<UTheOneAutoChessBattleComponent> AutoChessBattleComponentCache;

public:
	UFUNCTION(BlueprintCallable)
	void SpawnOneInteractActor(const TSubclassOf<ATheOneInteractActor> InteractActorClass, const FTransform& SpawnTransform);
	UFUNCTION(BlueprintCallable)
	ATheOneAIController* SpawnOneConfigAIAtTransform(const FTransform& SpawnTransform, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo);

	// Todo: 场景中放置一个可以移动的PlayerStart， 使用该位置作为生成位置， 这样不需要在FTheOneAIPawnSpawnInfo传入Transform
	UFUNCTION(BlueprintCallable)
	ATheOneAIController* SpawnOneAI(const TSubclassOf<ATheOneAIController> AIControllerClass, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo);
	UFUNCTION(BlueprintCallable)
	ATheOneAIController* SpawnOneAIAtTransform(const TSubclassOf<ATheOneAIController> AIControllerClass, const FTransform& SpawnTransform, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo);

	UFUNCTION(BlueprintCallable)
	void RemoveOneAI(ATheOneAIController* InAIController);
	
	UFUNCTION(BlueprintCallable)
	void RemoveOneAICharacter(ACharacter* InAICharacter);
	
	FString CreateAIName(int32 AIIndex);

protected:
	ATheOneAIController* SpawnOneAIInternal(TSubclassOf<ATheOneAIController> AIControllerClass, const FTransform& SpawnTransform, const FTheOneAIPawnSpawnInfo& PawnSpawnInfo, bool UseTransform);
	
	void RemoveOneAI(uint32 AIUniqueID);
	
	UFUNCTION(BlueprintImplementableEvent, META=(DisplayName="OnLevelPrepared"))
	void BP_OnLevelPrepared();

	////// 关卡配置字段 ///////
public:
	UPROPERTY(EditAnywhere)
	bool OverrideAICtrl = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAIController> OverrideAIControllerClass;
};


