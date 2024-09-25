// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeComponents/TheOneAutoChessBattleComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "TheOneEventSystem.generated.h"
// 命名规范:  蓝图动态事件用Signature结尾，C++事件用Delegate结尾
// 声明enum
enum class ETheOneUseAbilityCommandType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTheOneSaveLevel, const FName&, RowName, const FTheOneAutoChessLevelConfig, LevelData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectCharacterDelegate, ATheOneCharacterBase*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTheOneUseAbilityCommandSignature, ETheOneUseAbilityCommandType, CommandType, int32, IntPayload);
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
};
