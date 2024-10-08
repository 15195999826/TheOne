// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/TheOneUseAbilityCommandType.h"
#include "UObject/Interface.h"
#include "TheOneBattleInterface.generated.h"

UENUM(BlueprintType)
enum class ETheOneCharacterSkillState : uint8
{
	Idle,
	Casting
};

enum class ETheOneTryActiveResult : uint8;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTheOneBattleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEONE_API ITheOneBattleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BeforeEnterBattle() = 0;
	virtual void AfterEndBattle() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(ExpandEnumAsExecs = "Result"))
	class UTheOneAttackGA* DoAttack(ETheOneTryActiveResult& Result);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(ExpandEnumAsExecs = "Result"))
	class UTheOneGeneralGA* DoAbility(ETheOneUseAbilityCommandType InUseAbilityCommandType, int32 InIntPayload, AActor* InTargetActor, const FVector& InLocation, ETheOneTryActiveResult& Result);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsStun() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ETheOneCharacterSkillState GetSkillState() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetTargetActor() const;
	
	virtual void Die() = 0;
};
