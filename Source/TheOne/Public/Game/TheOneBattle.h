// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TheOneProjectileActor.h"
#include "Types/TheOneBattleContext.h"
#include "TheOneBattle.generated.h"

UENUM()
enum class ETheOneAIChoice : uint8
{
	EndTurn,
	WaitTurn, // 等待行动，放入当前行动队列最后
	RashMove, // 鲁莽移动，直接移动到玩家控制区
	EscapeMove, // 逃跑移动，尽量远离玩家
	ConservativeMove, // 保守移动，尽量靠近玩家但保持不进入玩家控制区
	ReleaseAbility, // 释放技能
};

USTRUCT()
struct FTheOneAIChoice
{
	GENERATED_BODY()

	FTheOneAIChoice(): Choice(), Score(-1), TargetCharacter(nullptr), AbilityIndex(0)
	{
	}

	ETheOneAIChoice Choice;

	double Score;

	TObjectPtr<ATheOneCharacterBase> TargetCharacter;

	FVector TargetLocation;

	int AbilityIndex;
};


class UTheOneBattleWindow;
class UTheOneTeamSystem;
class AHexGrid;
class ATheOneGameModeBase;
/**
 * 逻辑与表演不分离，这样使用GAS方便很多
 */
UCLASS()
class THEONE_API ATheOneBattle : public AActor
{
	GENERATED_BODY()

	
public:
	static ETheOneCamp GetOppositeCamp(ATheOneCharacterBase* Character);
	
	void OnEnterBattle();

	void BattleTick();

	TArray<ATheOneCharacterBase*> GetCharactersByCamp(ETheOneCamp InCamp) const;
protected:
	UPROPERTY(BlueprintReadOnly)
	FTheOneBattleContext BattleContext;
	
	UFUNCTION(BlueprintCallable)
	void CompleteWaitSignal() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnBattleStageChanged();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTheOneBattleWindow* GetBattleWindow() const;

	UFUNCTION()
	void BeforeCharacterMove(ATheOneCharacterBase* InCharacter);
	UFUNCTION()
	void AfterCharacterMove(ATheOneCharacterBase* InCharacter);
	
private:
	TMap<uint32, TWeakObjectPtr<ATheOneCharacterBase>> InBattleCharacters;
	
	FSimpleDelegate WaitSignal;
	int32 EnemyTeamID;
	
	void TeamMoveToBattleArea(const TArray<uint32>& Team, FRotator InRotation, int InStartRow, int InStartCol,
	                          ATheOneGameModeBase* GameMode, AHexGrid* HexGrid,
	                          UTheOneTeamSystem* TeamSystem);

	void WaitSignalChangeTo(ETheOneBattleStage NewStage);
	
	void NextRound();
	void UpdateControlTile();
	void NextCharacterTurn();
	
	void OnCharacterEndTurn(ATheOneCharacterBase* InCharacter);

	
};
