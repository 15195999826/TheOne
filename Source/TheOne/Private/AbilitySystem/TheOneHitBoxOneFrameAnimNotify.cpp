// Copyright Ogopogo


#include "AbilitySystem/TheOneHitBoxOneFrameAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UTheOneHitBoxOneFrameAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	bool IsRunningInEditorPreview = MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview;
	AActor* Owner = MeshComp->GetOwner();
	
	FVector Forward;
	if (IsRunningInEditorPreview)
	{
		Forward = FVector(0, 1, 0);
	} else
	{
		Forward = Owner->GetActorForwardVector();
	}
	// Todo: 这里StartLocation与EndLocation的AboveGround是相同的，可以做成函数
	
	// 获取角色的位置和旋转
	FVector MeshLocation = MeshComp->GetComponentLocation();
	FVector StartLocation = MeshLocation + FVector(0,0,1) * AboveGround + Forward * StartDistance;

	// 设置射线追踪的结束位置
	FVector EndLocation = MeshLocation + FVector(0,0,1) * AboveGround + Forward * EndDistance;

	// 设置需要忽略的Actor
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());

	// Trace Object Types
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	// 加入Pawn
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	if (HitMultiple)
	{
		TArray<AActor*> InRangeHitActors;
		// 扇形方位检测方式 : 先射线检测到圆形半径内的所有Actor， 再根据这些Actor的位置与圆心位置进行角度检测
		switch (HitBoxType)
		{
		case EHitBoxType::Box:
			BoxHits(IsRunningInEditorPreview, MeshComp, Owner->GetActorRotation(), StartLocation, EndLocation, ActorsToIgnore, ObjectTypes, InRangeHitActors);
			break;
		case EHitBoxType::Circle:
			CircleHits(MeshComp, StartLocation, ActorsToIgnore, ObjectTypes, InRangeHitActors);
			break;
		case EHitBoxType::Sector:
			SectorHits(MeshComp, Forward, StartLocation, ActorsToIgnore, ObjectTypes, InRangeHitActors);
			break;
		}
		
		// Make GameplayEventData
		if (IsRunningInEditorPreview)
		{
			return;
		}

		PostMontageEvent(Owner, InRangeHitActors);
	} else
	{
		if (HitBoxType == EHitBoxType::Circle || HitBoxType == EHitBoxType::Sector)
		{
			UE_LOG(LogTemp, Error, TEXT("[UPanguHitBoxOneFrameAnimNotify::Notify] SingleHit HitBoxType::Circle or Sector is not implemented yet."));
			return;
		}
		
		FHitResult OutHit;
		auto Orientation = Owner->GetActorRotation();
		if (!IsRunningInEditorPreview)
		{
			Orientation.Add(0, 90, 0);
		}
		if (UKismetSystemLibrary::BoxTraceSingleForObjects(MeshComp, StartLocation, EndLocation, BoxSize,
			Orientation, ObjectTypes, true,
			ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true,
			FLinearColor::Red, FLinearColor::Green, 0.5f))
		{
			if (IsRunningInEditorPreview)
			{
				return;
			}
		
			AActor* HitActor = OutHit.GetActor();
			if (HitActor)
			{
				PostMontageEvent(Owner, {HitActor});
			}
		}
	}
}

void UTheOneHitBoxOneFrameAnimNotify::BoxHits(bool IsRunningInEditorPreview, USkeletalMeshComponent* MeshComp,
	FRotator Orientation, const FVector& StartLocation, const FVector& EndLocation,
	TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
	TArray<AActor*>& InRangeHitActors)
{
	TArray<FHitResult> OutHits;

	if (!IsRunningInEditorPreview)
	{
		Orientation.Add(0, 90, 0);
	}
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(MeshComp, StartLocation, EndLocation, BoxSize,
	                                             Orientation , ObjectTypes, true,
	                                             ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits, true,
	                                             FLinearColor::Red, FLinearColor::Green, 0.5f);
	
	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			if (!InRangeHitActors.Contains(HitActor))
			{
				InRangeHitActors.Add(HitActor);
			}
			else
			{
				UKismetSystemLibrary::PrintString(
					MeshComp,
					"[UPanguHitBoxOneFrameAnimNotify::Notify] Hit Actor 多次.  HitActor: " + HitActor->
					GetName());
			}
		}
	}
}

void UTheOneHitBoxOneFrameAnimNotify::SectorHits(USkeletalMeshComponent* MeshComp, FVector Forward, FVector StartLocation, TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, TArray<AActor*>& InRangeHitActors)
{
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLocation, StartLocation, Radius,
	                                                 ObjectTypes, true,
	                                                 ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits,
	                                                 true,
	                                                 FLinearColor::Red, FLinearColor::Green, 0.5f);

	// 绘制圆心
	DrawDebugSphere(MeshComp->GetWorld(), StartLocation, 5, 10, FColor::Green, false, 0.5f);
	// 从圆心开始绘制扇形两条边界线， 扇形角度为SectorAngle
	FVector StartLine = StartLocation + Forward.RotateAngleAxis(SectorAngle / 2, FVector(0, 0, 1)) * Radius;
	FVector EndLine = StartLocation + Forward.RotateAngleAxis(-SectorAngle / 2, FVector(0, 0, 1)) * Radius;
	DrawDebugLine(MeshComp->GetWorld(), StartLocation, StartLine, FColor::Green, false, 0.5f);
	DrawDebugLine(MeshComp->GetWorld(), StartLocation, EndLine, FColor::Green, false, 0.5f);
			
	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			// 计算Actor与圆心的角度
			FVector HitLocation = HitActor->GetActorLocation();
			FVector Direction = HitLocation - StartLocation;
			Direction.Normalize();
			float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, Direction)));
			// 如果角度小于等于SectorAngle / 2， 则认为在扇形范围内
			if (Angle <= SectorAngle / 2)
			{
				// 检查是否已经在数组中
				if (!InRangeHitActors.Contains(HitActor))
				{
					InRangeHitActors.Add(HitActor);
					UKismetSystemLibrary::PrintString(MeshComp, "[UPanguHitBoxOneFrameAnimNotify::Notify] SectorHits HitActor: " + HitActor->GetName());
				}
				else
				{
					UKismetSystemLibrary::PrintString(
						MeshComp,
						"[UPanguHitBoxOneFrameAnimNotify::Notify] Hit Actor 多次.  HitActor: " + HitActor->
						GetName());
				}
			}
		}
	}
}

void UTheOneHitBoxOneFrameAnimNotify::CircleHits(USkeletalMeshComponent* MeshComp,
	FVector StartLocation, TArray<AActor*> ActorsToIgnore, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
	TArray<AActor*>& InRangeHitActors)
{
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLocation, StartLocation, Radius,
													 ObjectTypes, true,
													 ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits,
													 true,
													 FLinearColor::Red, FLinearColor::Green, 0.5f);
	// 绘制圆心
	DrawDebugSphere(MeshComp->GetWorld(), StartLocation, 5, 10, FColor::Green, false, 0.5f);

	for (const FHitResult& OutHit : OutHits)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			if (!InRangeHitActors.Contains(HitActor))
			{
				InRangeHitActors.Add(HitActor);
			}
			else
			{
				UKismetSystemLibrary::PrintString(
					MeshComp,
					"[UPanguHitBoxOneFrameAnimNotify::Notify] Hit Actor 多次.  HitActor: " + HitActor->
					GetName());
			}
		}
	}
}

void UTheOneHitBoxOneFrameAnimNotify::PostMontageEvent(AActor* Owner,const TArray<AActor*>& InRangeHitActors) const
{
	auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (ASC)
	{
		auto TheOneASC = Cast<UTheOneAbilitySystemComponent>(ASC);
		if (TheOneASC)
		{
			FTheOneMontageEventData MontageEventData;
			MontageEventData.TargetActors = InRangeHitActors;
			TheOneASC->HandleMontagePostEvent(Tag, MontageEventData);
		}
	}
}
