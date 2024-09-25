#pragma once

#include "CoreMinimal.h"

#include "TheOneProjectileConfig.generated.h"

class UNiagaraSystem;
class ATheOneProjectileActor;

UENUM(BlueprintType)
enum class ETheOneProjectileFlyType : uint8
{
	Invalid UMETA(DisplayName="无效"),
	Trace UMETA(DisplayName="追踪目标角色"),
	FixedLocation UMETA(DisplayName="固定位置"),
	Lob UMETA(DisplayName="抛射"),
	Line UMETA(DisplayName="直线"),
	Instant UMETA(DisplayName="瞬发(射线)"),
};

/**
 * 目标： Actor 或者 一个位置
 *飞行类型： 直线， 抛物线， 射线
 *是否追踪目标， 仅对直线、抛物线且目标为Actor， 才可配置
 *可以命中的阵营： 友方， 敌方， 中立
 *命中后是否销毁
 */
USTRUCT(BlueprintType)
struct FTheOneProjectileConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATheOneProjectileActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Actor设置", meta=(DisplayName="Mesh"))
	TSoftObjectPtr<UStaticMesh> Mesh; // Mesh
	
	UPROPERTY(EditAnywhere, Category="Actor设置", meta=(DisplayName="主特效"))
	TSoftObjectPtr<UNiagaraSystem> MainEffect; // 主特效

	UPROPERTY(EditAnywhere, Category="Actor设置", meta=(DisplayName="拖尾特效"))
	TSoftObjectPtr<UNiagaraSystem> TrailEffect; // 拖尾特效

	UPROPERTY(EditAnywhere, Category="Actor设置", meta=(DisplayName="爆炸特效"))
	TSoftObjectPtr<UNiagaraSystem> ImpactEffect; // 爆炸特效
};