// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneDelegates.h"
#include "GameModeComponents/TheOneAutoChessBattleComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TheOneBattleContext.h"
#include "Types/TheOneFocusData.h"
#include "TheOneEventSystem.generated.h"

// 命名规范:  蓝图动态事件用Signature结尾，C++事件用Delegate结尾
// 声明enum
enum class ETheOneUseAbilityCommandType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTheOneSaveLevel, const FName&, RowName, const FTheOneAutoChessLevelConfig, LevelData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectCharacterDelegate, ATheOneCharacterBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGetFocusDelegate, const FTheOneFocusData&);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTheOneCharacterSignature, ATheOneCharacterBase*, InCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTheOneOneBoolSignature, bool, BoolPayload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTheOneUseAbilityCommandSignature, const FTheOneUseAbilityCommandPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTheOneSimpleSignature);
/**
 * 
 */
UCLASS(MinimalAPI)
class UTheOneEventSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable)
	FOnTheOneSaveLevel OnTheOneSaveLevel;

	FOnSelectCharacterDelegate OnSelectCharacter;
	FSimpleMulticastDelegate OnDeSelectCharacter;

	UPROPERTY(BlueprintCallable)
	FTheOneUseAbilityCommandSignature UseAbilityCommand;

	UPROPERTY(BlueprintAssignable)
	FTheOneUseAbilityCommandSignature OnAbilityCommandWaiting;
	UPROPERTY(BlueprintAssignable)
	FTheOneSimpleSignature OnAbilityCommandFinished;
	UPROPERTY(BlueprintAssignable)
	FTheOneSimpleSignature OnAbilityCommandCanceled;
	UPROPERTY(BlueprintAssignable)
	FTheOneSimpleSignature OnAbilityCommandErrorTarget;

	FOnGetFocusDelegate OnGetFocus;
	FSimpleMulticastDelegate OnLoseFocus;
	
	FOnTheOneOneIntParamDelegate OnSelectInBagCharacter;

	FOnSelectCharacterDelegate OnCharacterGetTurn;
	UPROPERTY(BlueprintCallable)
	FOnTheOneCharacterSignature OnCharacterEndTurn;

	// For Battle Start
	FOnTheOneOneIntParamDelegate OnRoundBegin;

	UPROPERTY(BlueprintCallable)
	FTheOneOneBoolSignature OnCommandBehaviorEnd;

	UPROPERTY(BlueprintCallable)
	FOnTheOneCharacterSignature NativeBeforeCharacterMove;
	UPROPERTY(BlueprintCallable)
	FOnTheOneCharacterSignature NativeAfterCharacterMove;
	UPROPERTY(BlueprintAssignable)
	FOnTheOneCharacterSignature OnCharacterDead;
};
