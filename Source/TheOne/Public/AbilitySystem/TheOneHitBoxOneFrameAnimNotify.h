// Copyright Ogopogo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TheOneHitBoxOneFrameAnimNotify.generated.h"

// enum 射线追踪的类型， 扇形、圆形、矩形
UENUM(BlueprintType)
enum class EHitBoxType : uint8
{
	Box,
	Circle,
	Sector
};

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneHitBoxOneFrameAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool HitMultiple = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EHitBoxType HitBoxType;

	// Box, 仅在HitBoxType为Box时生效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "HitBoxType == EHitBoxType::Box"))
	FVector BoxSize = FVector(50, 50, 50);

	// Sector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "HitBoxType == EHitBoxType::Sector"))
	float SectorAngle = 90;

	// Sector or Circle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (EditCondition = "HitBoxType == EHitBoxType::Sector || HitBoxType == EHitBoxType::Circle"))
	float Radius = 100;
	
	// AboveGround
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float AboveGround = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float StartDistance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float EndDistance = 100;

	void BoxHits(bool bIsRunningInEditorPreview, USkeletalMeshComponent* MeshComp, FRotator Rotator, const FVector& Vector, const FVector& EndLocation, TArray<AActor*> Array, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
				TArray<AActor*>& InRangeHitActors);
	void SectorHits(USkeletalMeshComponent* MeshComp, FVector Forward, FVector StartLocation, TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes, TArray<AActor*>& InRangeHitActors);
	void CircleHits(USkeletalMeshComponent* MeshComp, FVector StartLocation, TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes, TArray<AActor*>& InRangeHitActors);

	void PostMontageEvent(AActor* Owner,const TArray<AActor*>& InRangeHitActors) const;
};
