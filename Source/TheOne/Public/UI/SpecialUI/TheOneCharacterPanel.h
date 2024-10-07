// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Interface/TheOneShortcutInterface.h"
#include "Item/TheOneItemSystem.h"
#include "TheOneCharacterPanel.generated.h"

class UTheOneAttributeLine;
class UTheOneAbilityWidget;
class UTheOneItemUserWidget;
class UTheOneGridSlot;
class UProgressBar;
class ATheOneCharacterBase;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneCharacterPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> ActionPointLine;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> EnergyLine;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> WillLine;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> HeadArmorLine;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> BodyArmorLine;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> HPLine;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> NameTextBlock;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakCacheCharacter;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Bind(ATheOneCharacterBase* InTheOneCharacterBase);
	void UnBind();
};
