#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TheOneGameplayTags.h"
#include "TheOneAttrModifier.generated.h"

USTRUCT(BlueprintType)
struct FTheOneAttrModifier
{
	GENERATED_BODY()

	FTheOneAttrModifier(): Value(0), bIsPercent(false)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta=(GetOptions="TheOne.TheOneStructDeveloperSettings.GetAttributeTagValues"))
	FName AttrTagName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsPercent;
};