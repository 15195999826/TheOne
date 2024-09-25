// Copyright Ogopogo

#pragma once

#include "CoreMinimal.h"
#include "UI/TheOneViewController.h"
#include "HeadStateBarController.generated.h"

class UAbilitySystemComponent;
class UTheOneLifeAttributeSet;
class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS(BlueprintType)
class THEONE_API UHeadStateBarController : public UTheOneViewController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupAbility(UAbilitySystemComponent* InASC, UTheOneLifeAttributeSet* InAS);
	
	UFUNCTION(BlueprintCallable)
	void Setup(UProgressBar* HPBar, UProgressBar* ManaBar);

private:
	float Health;
	float MaxHealth;
	float Mana;
	float MaxMana;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UTheOneLifeAttributeSet> AS;
	UPROPERTY()
	TObjectPtr<UProgressBar> HPProgressBar;
	UPROPERTY()
	TObjectPtr<UProgressBar> ManaProgressBar;

	void UpdateHP() const;
	void UpdateMana() const;
};
