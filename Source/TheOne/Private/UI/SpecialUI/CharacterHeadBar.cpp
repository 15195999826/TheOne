// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/CharacterHeadBar.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Components/ProgressBar.h"

void UCharacterHeadBar::Bind(UAbilitySystemComponent* InASC)
{
	WeakASCCache = InASC;
	Health = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetHealthAttribute());
	MaxHealth = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetMaxHealthAttribute());
	HeadArmor = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetHeadArmorAttribute());
	MaxHeadArmor = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetMaxHeadArmorAttribute());
	BodyArmor = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetBodyArmorAttribute());
	MaxBodyArmor = InASC->GetNumericAttribute(UTheOneLifeAttributeSet::GetMaxBodyArmorAttribute());
	// 初始化血条
	UpdateHP();
	UpdateHeadArmor();
	UpdateBodyArmor();
	
	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		Health = Data.NewValue;
		UpdateHP();
	});
	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		MaxHealth = Data.NewValue;
		UpdateHP();
	});
	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetBodyArmorAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		BodyArmor = Data.NewValue;
		UpdateBodyArmor();
	});
	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxBodyArmorAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		MaxBodyArmor = Data.NewValue;
		UpdateBodyArmor();
	});

	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetHeadArmorAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		HeadArmor = Data.NewValue;
		UpdateHeadArmor();
	});

	InASC->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxHeadArmorAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		MaxHeadArmor = Data.NewValue;
		UpdateHeadArmor();
	});
}

void UCharacterHeadBar::UnBind()
{
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetHealthAttribute()).RemoveAll(this);
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetBodyArmorAttribute()).RemoveAll(this);
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxBodyArmorAttribute()).RemoveAll(this);
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetHeadArmorAttribute()).RemoveAll(this);
	WeakASCCache->GetGameplayAttributeValueChangeDelegate(UTheOneLifeAttributeSet::GetMaxHeadArmorAttribute()).RemoveAll(this);
	WeakASCCache = nullptr;
}

void UCharacterHeadBar::UpdateHP()
{
	if (FMath::IsNearlyZero(MaxHealth))
	{
		HpBar->SetPercent(0);
		return;
	}
	HpBar->SetPercent(Health / MaxHealth);
}

void UCharacterHeadBar::UpdateHeadArmor()
{
	if (FMath::IsNearlyZero(MaxHeadArmor))
	{
		HeadArmorBar->SetPercent(0);
		return;
	}
	HeadArmorBar->SetPercent(HeadArmor / MaxHeadArmor);
}

void UCharacterHeadBar::UpdateBodyArmor()
{
	if (FMath::IsNearlyZero(MaxBodyArmor))
	{
		BodyArmorBar->SetPercent(0);
		return;
	}
	BodyArmorBar->SetPercent(BodyArmor / MaxBodyArmor);
}
