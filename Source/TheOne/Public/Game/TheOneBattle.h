// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TheOneProjectileActor.h"
#include "Types/TheOneBattleContext.h"
#include "TheOneBattle.generated.h"

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

	static ETheOneCamp GetOppositeCamp(ATheOneCharacterBase* Character);
	
public:
	void OnEnterBattle();

	void BattleTick();

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
	void NextCharacterTurn();
	
	void OnCharacterEndTurn(ATheOneCharacterBase* InCharacter);
};
