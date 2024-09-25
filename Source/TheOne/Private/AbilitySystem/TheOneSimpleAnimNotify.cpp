// Copyright Ogopogo


#include "AbilitySystem/TheOneSimpleAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"

void UTheOneSimpleAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	bool isRunningInEditorPreview = MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview;
	AActor* Owner = MeshComp->GetOwner();

	if (isRunningInEditorPreview)
	{
		return;
	}
	
	auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (ASC)
	{
		auto PanguASC = Cast<UTheOneAbilitySystemComponent>(ASC);
		if (PanguASC)
		{
			PanguASC->HandleMontagePostEvent(Tag, FTheOneMontageEventData()); 
		}
	}
}
