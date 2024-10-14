#pragma once
#include "CoreMinimal.h"
#include "TheOneLogChannels.h"

#include "TheOneBattleContext.generated.h"
class ATheOneCharacterBase;

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

	inline static TMap<ETheOneAIChoice, FString> ChoiceNameMap = {
		{ETheOneAIChoice::EndTurn, "EndTurn"},
		{ETheOneAIChoice::WaitTurn, "WaitTurn"},
		{ETheOneAIChoice::RashMove, "RashMove"},
		{ETheOneAIChoice::EscapeMove, "EscapeMove"},
		{ETheOneAIChoice::ConservativeMove, "ConservativeMove"},
		{ETheOneAIChoice::ReleaseAbility, "ReleaseAbility"},
	};

	FTheOneAIChoice(): Choice(), Score(-1), TargetCharacter(nullptr), AbilityIndex(0)
	{
	}

	ETheOneAIChoice Choice;

	double Score;

	TObjectPtr<ATheOneCharacterBase> TargetCharacter;

	FVector TargetLocation;

	int AbilityIndex;

	FString ToString() const;
};


UENUM(BlueprintType)
enum class ETheOneBattleStage : uint8
{
	None = 0,
	EnterNewRound = 1,
	NewRoundPending = 2,
	EnterCharacterTurn = 3,
	CharacterTurn = 4,
	
	GameOver = 99,
};

USTRUCT(BlueprintType)
struct FTheOneMapContext
{
	GENERATED_BODY()

	FTheOneMapContext()
	{
	}
	
};

// 战斗中各个棋子的上下文
USTRUCT(BlueprintType)
struct FTheOneChessContext
{
	GENERATED_BODY()

	FTheOneChessContext()
	{
	}

	// 记录当轮执行过的行动
	TArray<FTheOneAIChoice> Memory;
};

USTRUCT(BlueprintType)
struct FTheOneBattleContext
{
	GENERATED_BODY()

	FTheOneBattleContext(): Stage(), Round(0), CurrentTurn(-1)
	{
	}

	UPROPERTY(BlueprintReadWrite)
	bool WantToGameOver = false;
	UPROPERTY(BlueprintReadWrite)
	bool WantToNextAIThink = false;
	UPROPERTY(BlueprintReadOnly)
	ETheOneBattleStage Stage;
	UPROPERTY(BlueprintReadOnly)
	int Round;
	UPROPERTY(BlueprintReadOnly)
	int CurrentTurn;
	UPROPERTY(BlueprintReadOnly)
	TArray<ATheOneCharacterBase*> ActionQueue;

	TMap<uint32,FTheOneChessContext> ChessContextMap;

	FTheOneMapContext MapContext;

private:
	bool EnterNewStage = false;

public:
	void SetStage(ETheOneBattleStage InStage)
	{
		if (Stage == InStage)
		{
			UE_LOG(LogTheOne, Error, TEXT("SetStage: Stage is already %d"), (int)InStage);
			return;
		}

		if (EnterNewStage)
		{
			UE_LOG(LogTheOne, Error, TEXT("尚未处理上一个阶段的逻辑"));
			return;
		}
		
		Stage = InStage;
		EnterNewStage = true;
	}

	void ResolveStage()
	{
		check(EnterNewStage);
		EnterNewStage = false;
	}

	bool ShouldResolveNewStage() const
	{
		return EnterNewStage;
	}
};