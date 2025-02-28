// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheOneEventSystem.h"
#include "GameFramework/PlayerController.h"
#include "Types/TheOneUseAbilityCommandType.h"
#include "TheOnePlayerControllerBase.generated.h"


UENUM(BlueprintType)
enum class ETheOneCursorState : uint8
{
	Normal,
	SelectActor,
	SelectGround,
};

UENUM(BlueprintType)
enum class ETheOneSelectActorType : uint8
{
	Any,
	Ally,
	Enemy,
	// Todo: 自定义目标选择？ 还要再加一个字段Tag表示？
	Special,
};

class URegisterInputActionTask;
class ATheOneCharacterBase;
/**
 * 
 */
UCLASS()
class THEONE_API ATheOnePlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

protected:
	FCollisionResponseParams ResponseParam = FCollisionResponseParams(ECR_Overlap);
	TArray<FHitResult> HitResults;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ATheOneCharacterBase> SelectedCharacter;
	
	bool bIsPawnImplementICursorTrace = false;

	// Todo: 需要做到当前队列中存在某一个Consume消耗此输入时，其它Action不再触发， 用于实现游戏内不同情况下按ESC结果不同的情况
	TMap<int, int> InputID2IndexMap;
	TArray<URegisterInputActionTask*> InputTasks;
	bool OverWidget;

public:
	virtual void BeginPlay() override;
	void DeselectCharacter();
	virtual void Tick(float DeltaSeconds) override;
	void CursorTrace();

	void RegisterInputAction(URegisterInputActionTask* InTask);
	void UnregisterInputAction(const int& InTaskID);

protected:
	virtual void GeneralOnHitGround(const FVector& InHitLocation, FVector& OutGroundLocation);
	virtual void GeneralOnHitCharacter(ATheOneCharacterBase* HitCharacter);
	virtual void GeneralOnHitNone();
	
	virtual bool CanWalk(const FVector& InLocation) const { return true; }
	
	UFUNCTION(BlueprintNativeEvent)
	void BP_OnHitGround(const FVector& HitLocation, bool bIsRightClick, bool bIsLeftClick, bool CanWalk);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnHitActor(AActor* InHitActor, bool bIsRightClick);
	// ForCursorState
protected:
	ETheOneCursorState CursorState = ETheOneCursorState::Normal;
	ETheOneSelectActorType SelectActorType = ETheOneSelectActorType::Any;
	void ChangeCursorState(ETheOneCursorState InState, ETheOneSelectActorType InSelectActorType = ETheOneSelectActorType::Any, int InReleaseDistance = 0);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCursorStateChange(ETheOneCursorState InState, ETheOneSelectActorType InSelectActorType, int InReleaseDistance);

	// For UseAbilityCommand
protected:
	bool HasUseAbilityCommandCache = false;
	FTheOneUseAbilityCommandPayload PayloadCache;
	int ReleaseDistanceCache;
	float CostCache;
	
	UFUNCTION()
	virtual void ReceiveUseAbilityCommand(const FTheOneUseAbilityCommandPayload& Payload);
	UFUNCTION()
	virtual void OnAbilityCommandCanceled();
	UFUNCTION()
	virtual void OnAbilityCommandFinished();

	virtual void ConsumeActionPoint(ATheOneCharacterBase* InCharacter, float InCost);

	virtual bool InReleaseDistance(const FVector& SourceLocation, const FVector& TargetLocation, int InReleaseDistance);

	virtual void ShowReleaseDistanceTips();
	UFUNCTION()
	virtual void HideReleaseDistanceTips();

	virtual ATheOneCharacterBase* GetExecCharacter();
};
