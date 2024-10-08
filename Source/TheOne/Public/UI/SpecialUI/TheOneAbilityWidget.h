// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TheOneShortcutInterface.h"
#include "TheOneAbilityWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExecuteAbility, int, PayLoad);

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAbilityWidget : public UUserWidget, public ITheOneShortcutInterface
{
	GENERATED_BODY()

	inline static TMap<int, FKey> KeyIndex2KeyMap = {
		{0, EKeys::One},
		{1, EKeys::Two},
		{2, EKeys::Three},
		{3, EKeys::Four},
		{4, EKeys::Five},
		{5, EKeys::Six},
		{6, EKeys::Seven},
		{7, EKeys::Eight},
		{8, EKeys::Nine},
		{9, EKeys::Zero},
	};

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CDCover;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CDText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn))
	int IntPayload;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	FKey ShortcutKey;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnExecuteAbility OnExecuteAbility;
	
	void BindAbilityData(UTheOneGeneralGA* GA, int InPayload);

	void UnBindAbilityData();
};
