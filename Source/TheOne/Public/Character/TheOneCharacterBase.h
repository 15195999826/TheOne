// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "TheOneBattleInterface.h"
#include "GameFramework/Character.h"
#include "HexGrid/InHexActorInterface.h"
#include "Types/TheOneCharacterConfigStruct.h"
#include "TheOneCharacterBase.generated.h"

class UTheOneLifeAttributeSet;
class UTheOneDataDrivePassiveGA;
class UTheOneGeneralGA;
class UTheOneAbilitySystemComponent;
class UWidgetComponent;
class UTheOneGameplayAbility;
class UTheOneAttributeSet;

USTRUCT()
struct FTheOneAbilityCache
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	
	UPROPERTY()
	TWeakObjectPtr<UTheOneGeneralGA> AbilityGA;
};

DECLARE_MULTICAST_DELEGATE(FOnTheOneCharacterDeadSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterEnterNewCoordSignature, const FHCubeCoord&, InCoord);
UCLASS()
class THEONE_API ATheOneCharacterBase : public ACharacter, public IAbilitySystemInterface, public IInHexActorInterface, public ITheOneBattleInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ATheOneCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(BlueprintAssignable)
	FOnCharacterEnterNewCoordSignature OnEnterNewCoord;

public:
	TWeakObjectPtr<USkeletalMeshComponent> OutlookMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HeadBarWidgetComponent;
	
	FOnTheOneCharacterDeadSignature OnceOnDead;

	UPROPERTY(BlueprintReadOnly)
	FName ConfigRowName;

	UPROPERTY(BlueprintReadOnly)
	FName DefaultWeaponRow;

	UPROPERTY(BlueprintReadOnly)
	int32 MainHandItemID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 OffHandItemID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 HeadItemID = INDEX_NONE;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ClothItemID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 LeftJewelryItemID = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 RightJewelryItemID = INDEX_NONE;
	
	TMap<int32, TArray<FTheOneAbilityCache>> AbilityCaches;
	
	// 角色自带技能Map
	TMap<FGameplayAbilitySpecHandle, TWeakObjectPtr<UTheOneGeneralGA>> CharacterAbilityMap;

	// 数据驱动被动技能
	FGameplayAbilitySpecHandle DataDrivePassiveSpecHandle;
	TWeakObjectPtr<UTheOneDataDrivePassiveGA> DataDrivePassiveGA;
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TWeakObjectPtr<UTheOneAbilitySystemComponent> TheOneAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UTheOneAttributeSet> AttributeSet;
	
	UPROPERTY()
	TObjectPtr<UTheOneLifeAttributeSet> LifeAttributeSet;

	UPROPERTY(BlueprintReadOnly)
	ETheOneCamp Camp;

	UPROPERTY()
	ETheOneCharacterSkillState SkillState;

	FHCubeCoord CurrentCoord; // 当前所在的格子
	FHCubeCoord BookedCoord; // 将要前往的下一个格子

	// virtual Start
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	UTheOneAbilitySystemComponent* GetTheOneAbilitySystemComponent() const
	{
		return TheOneAbilitySystemComponent.Get();
	}

	
	virtual void BeforeEnterBattle() override;
	virtual void AfterEndBattle() override;
	virtual UTheOneAttackGA* DoAttack_Implementation(ETheOneTryActiveResult& Result) override;
	/**
	 * InUseAbilityCommandType == ETheOneUseAbilityCommandType::UseWeaponAbility, InIntPayload == 1, 2, 3
	 * @param InUseAbilityCommandType
	 * @param InIntPayload
	 * @param InTargetActor
	 * @param InLocation
	 * @param Result
	 * @return
	 */
	virtual UTheOneGeneralGA* DoAbility_Implementation(ETheOneUseAbilityCommandType InUseAbilityCommandType, int32 InIntPayload, AActor* InTargetActor, const FVector& InLocation, ETheOneTryActiveResult& Result) override;
	virtual bool IsDead_Implementation() const override;
	virtual bool IsStun_Implementation() const override;

	virtual AActor* GetTargetActor_Implementation() const override;
	
	virtual void Die() override;
	
	virtual void OnSelected();
	virtual void OnDeselected();

	virtual const FHCubeCoord& GetCurrentHexCoord() const override;
	virtual const FHCubeCoord& GetBookedHexCoord() const override;

	virtual void SetCurrentHexCoord(const FHCubeCoord& InCoord) override;
	virtual void SetBookedHexCoord(const FHCubeCoord& InCoord) override;

	virtual ETheOneCamp GetCamp_Implementation() override
	{
		return Camp;
	}

	virtual UMeshComponent* GetRootMesh_Implementation() override
	{
		return GetMesh();
	}

	virtual ETheOneCharacterSkillState GetSkillState_Implementation() const override
	{
		return SkillState;
	}


	virtual uint32 GetFlag() const override;

protected:
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	// virtual End

public:
	/**
	 * 监听移速、旋转速度变化同时更新MovementComponent的数据
	 */
	void SpawnInit();
	
	UFUNCTION(BlueprintCallable)
	UTheOneAttributeSet* GetAttributeSet() const
	{
		return AttributeSet;
	}

	UFUNCTION(BlueprintCallable)
	UTheOneLifeAttributeSet* GetLifeAttributeSet() const
	{
		return LifeAttributeSet;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FTheOneCharacterConfig& GetConfig() const;

	void SetCamp(ETheOneCamp InCamp)
	{
		Camp = InCamp;
		BP_OnSetCamp(InCamp);
	}

	
	void SetSkillState(ETheOneCharacterSkillState InSkillState)
	{
		SkillState = InSkillState;
	}

	const FTheOneAbilityCache* GetAbilityCacheByIntPayload(int32 InIntPayload);
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Die"))
	void BP_Die();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnSetCamp"))
	void BP_OnSetCamp(ETheOneCamp InCamp);
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnSelected"))
	void BP_OnSelected();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeselected"))
	void BP_OnDeselected();
	
private:
	void OnMoveSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void OnRotationRateChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

	// For Turn Based Battle
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnGetTurn();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnEndTurn();
	
private:
	void OnGetTurn(ATheOneCharacterBase* InCharacter);
	void OnEndTurn(ATheOneCharacterBase* InCharacter);
};
