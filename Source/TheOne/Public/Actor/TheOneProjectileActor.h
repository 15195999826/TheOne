// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ITheOneCharacterSpawnActor.h"
#include "ITheOneHitTarget.h"
#include "AbilitySystem/TheOneAbilityAction.h"
#include "GameFramework/Actor.h"
#include "TheOneProjectileActor.generated.h"

class UNiagaraComponent;
enum class ETheOneCamp : uint8;
class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class THEONE_API ATheOneProjectileActor : public AActor, public IAbilitySystemInterface, public ITheOneCharacterSpawnActor, public ITheOneHitTarget
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheOneProjectileActor();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual ATheOneCharacterBase* GetSourceCharacter() const override;
	virtual AActor* GetHitTarget() const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnHitTarget();
	virtual void Tick(float DeltaSeconds) override;
public:	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AActor> SourceActor;
	
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UTheOneGameplayAbility> SourceAbility;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> MainEffectComponent;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY()
	FVector DstLocation;

	UPROPERTY()
	FTheOneAbilityActionContainer HitActionContainer;
	
public:
	// 优先追踪TargetActor， 当TargetActor为空时，追踪DstLocation
	void SetUp(const FTheOneProjectileConfig* InConfigRow,
	           // 源头相关
	           AActor* InSourceActor, UTheOneGameplayAbility* InAbility,
	           // 目标相关
	           AActor* InTargetActor, const FVector& InDstLocation,
	           // 配置相关
	           ETheOneProjectileFlyType InFlyType, const float InSpeed,
	           // 功能相关
	           const TArray<FTheOneAbilityActionData*>& InHitActionData);
	
};
