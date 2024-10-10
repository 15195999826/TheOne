#pragma once
#include "CoreMinimal.h"
#include "TheOneLogChannels.h"

#include "TheOneBattleContext.generated.h"
class ATheOneCharacterBase;

UENUM(BlueprintType)
enum class ETheOneBattleStage : uint8
{
	None = 0,
	EnterNewRound = 1,
	NewRoundPending = 2,
	EnterCharacterTurn = 3,
	CharacterTurn = 4,
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
	
};

USTRUCT(BlueprintType)
struct FTheOneBattleContext
{
	GENERATED_BODY()

	FTheOneBattleContext(): Stage(), Round(0), CurrentTurn(-1)
	{
	}

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