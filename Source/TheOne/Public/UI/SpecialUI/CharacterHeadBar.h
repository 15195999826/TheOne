// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHeadBar.generated.h"

class UAbilitySystemComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class THEONE_API UCharacterHeadBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HeadArmorBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> BodyArmorBar;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HpBar;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> WeakASCCache;
	
	UFUNCTION(BlueprintCallable)
	void Bind(UAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable)
	void UnBind();

private:
	float Health;
	float MaxHealth;
	float HeadArmor;
	float MaxHeadArmor;
	float BodyArmor;
	float MaxBodyArmor;
	void UpdateHP();
	void UpdateHeadArmor();
	void UpdateBodyArmor();
};
