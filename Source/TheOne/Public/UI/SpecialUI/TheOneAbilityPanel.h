// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/TheOneCharacterBase.h"
#include "TheOneAbilityPanel.generated.h"

class UHorizontalBox;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAbilityPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	TSubclassOf<class UTheOneAbilityWidget> AbilityWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AbilityWidgetContainer;

	void UnBind();
	
	void Bind(ATheOneCharacterBase* InCharacter);

protected:
	UPROPERTY()
	TArray<TObjectPtr<UTheOneAbilityWidget>> AbilityWidgetPool;

	UTheOneAbilityWidget* GetAbilityWidget();
	void ReleaseAbilityWidget(UTheOneAbilityWidget* InWidget);

	UFUNCTION()
	void OnExecuteAbility(int InIntPayLoad);
};
