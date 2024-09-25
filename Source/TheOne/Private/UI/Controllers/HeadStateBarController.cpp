// Copyright Ogopogo


#include "UI/Controllers/HeadStateBarController.h"

#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHeadStateBarController::SetupAbility(UAbilitySystemComponent* InASC, UTheOneLifeAttributeSet* InAS)
{
	ASC = InASC;
	AS = InAS;
}

void UHeadStateBarController::Setup(UProgressBar* HPBar, UProgressBar* ManaBar)
{
	// 初始化血条
	Health = AS->GetHealth();
	MaxHealth = AS->GetMaxHealth();
	Mana = AS->GetMana();
	MaxMana = AS->GetMaxMana();

	this->HPProgressBar = HPBar;
	this->ManaProgressBar = ManaBar;

	UpdateHP();
	UpdateMana();
	
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		Health = Data.NewValue;
		UpdateHP();
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		MaxHealth = Data.NewValue;
		UpdateHP();
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		Mana = Data.NewValue;
		UpdateMana();
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		MaxMana = Data.NewValue;
		UpdateMana();
	});
}

void UHeadStateBarController::UpdateHP() const
{
	HPProgressBar->SetPercent(Health / MaxHealth);
}

void UHeadStateBarController::UpdateMana() const
{
	ManaProgressBar->SetPercent(Mana / MaxMana);
}
