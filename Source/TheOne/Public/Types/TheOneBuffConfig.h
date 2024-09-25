#pragma once
#include "CoreMinimal.h"

#include "TheOneBuffConfig.generated.h"

USTRUCT(BlueprintType)
struct FTheOneBuffConfig : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Buff设置", meta=(DisplayName="Buff名称"))
	FName BuffName;

	UPROPERTY(EditAnywhere, Category="Buff设置", meta=(DisplayName="Buff描述"))
	FText BuffDesc;

	UPROPERTY(EditAnywhere, Category="Buff设置", meta=(DisplayName="Buff图标"))
	TSoftObjectPtr<UTexture2D> BuffIcon;
};
