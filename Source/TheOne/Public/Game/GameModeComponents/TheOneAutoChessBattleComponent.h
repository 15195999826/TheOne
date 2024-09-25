// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/TheOneCharacterConfigStruct.h"
#include "Components/ActorComponent.h"
#include "Item/TheOneItemSystem.h"
#include "TheOneAutoChessBattleComponent.generated.h"

class ATheOneAutoChessAIController;
class ATheOneGameModeBase;

/**
 * AI 生成，移除，查询
 * 一般放在GS里调用这些函数
 */
UCLASS(Blueprintable, ClassGroup=(TheOneGameModeComponent), meta=(BlueprintSpawnableComponent) )
class THEONE_API UTheOneAutoChessBattleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTheOneAutoChessBattleComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<ATheOneAIController> DefaultAIControllerClass;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<ATheOneAutoChessAIController>> EmptyCtrls;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<ATheOneAutoChessAIController>> EnemyCtrls;

	UPROPERTY()
	TArray<TWeakObjectPtr<ATheOneAutoChessAIController>> PlayerCtrls;
	
public:

protected:
	bool bTicking = false;

public:
	UFUNCTION(BlueprintCallable)
	void StartBattle();
	
	const TArray<TWeakObjectPtr<ATheOneAutoChessAIController>>& GetEnemies(ATheOneCharacterBase* InTheOneCharacter);

protected:

	void OnLevelPrepared();
	void BattleTick(float DeltaTime);

	void OnBattleOver(bool InPlayerWin);
	void OnAICharacterDead();
	ATheOneGameModeBase* GetGameMode() const;


private:

};
