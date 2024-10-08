// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAttributeLine.h"

#include "TheOneGameplayTags.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// 初始化Tag2Attribute
TMap<FGameplayTag, TFunction<FGameplayAttribute()>> UTheOneAttributeLine::Tag2Attribute = {
	{TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxHealth, [](){return UTheOneLifeAttributeSet::GetMaxHealthAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_Health, [](){return UTheOneLifeAttributeSet::GetHealthAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_MaxHeadArmor,[](){return UTheOneLifeAttributeSet::GetMaxHeadArmorAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_HeadArmor, [](){return UTheOneLifeAttributeSet::GetHeadArmorAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_MaxBodyArmor, [](){return UTheOneLifeAttributeSet::GetMaxBodyArmorAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_BodyArmor, [](){return UTheOneLifeAttributeSet::GetBodyArmorAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxActionPoint,[](){return UTheOneAttributeSet::GetMaxActionPointAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_ActionPoint, [](){return UTheOneAttributeSet::GetActionPointAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Vital_MaxEnergy, [](){return UTheOneAttributeSet::GetMaxEnergyAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Runtime_Energy, [](){return UTheOneAttributeSet::GetEnergyAttribute();}},
	{TheOneGameplayTags::SetByCaller_Attribute_Vital_Speed, [](){return UTheOneAttributeSet::GetSpeedAttribute();}},
};


void UTheOneAttributeLine::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (TheOneGameplayTags::Attributes2Text.Contains(AttributeTag))
	{
		AttributeName->SetText(TheOneGameplayTags::Attributes2Text[AttributeTag]);
	}
	else
	{
		AttributeName->SetText(FText::FromString(AttributeTag.ToString()));
	}
	
	if (Tag2Attribute.Contains(MaxAttributeTag))
	{
		AttributeSpace->SetVisibility(ESlateVisibility::Visible);
		MaxAttributeValue->SetVisibility(ESlateVisibility::Visible);
		if (bShowProgressBar)
		{
			AttributeProgressBar->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AttributeProgressBar->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		AttributeSpace->SetVisibility(ESlateVisibility::Collapsed);
		MaxAttributeValue->SetVisibility(ESlateVisibility::Collapsed);
		AttributeProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTheOneAttributeLine::RemoveListeners(const FGameplayAttribute& Attribute)
{
	if (WeakASCCache.IsValid())
	{
		WeakASCCache->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		if (Tag2Attribute.Contains(MaxAttributeTag))
		{
			const auto& MaxAttribute = Tag2Attribute[MaxAttributeTag]();
			WeakASCCache->GetGameplayAttributeValueChangeDelegate(MaxAttribute).RemoveAll(this);
		}
	}
}

void UTheOneAttributeLine::Bind(UAbilitySystemComponent* InASC)
{
	if (!Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}

	const FGameplayAttribute& Attribute = Tag2Attribute[AttributeTag]();
	
	RemoveListeners(Attribute);
	
	WeakASCCache = InASC;

	// 直接进行一次更新
	bool Found = false;
	auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(Attribute, Found);
	AttributeValue->SetText(FText::AsNumber(CurrentValue));

	WeakASCCache->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UTheOneAttributeLine::OnAttributeChanged);
	if (Tag2Attribute.Contains(MaxAttributeTag))
	{
		bHasMaxAttribute = true;
		const auto& MaxAttribute = Tag2Attribute[MaxAttributeTag]();
		Found = false;
		auto CurrentMaxValue = WeakASCCache->GetGameplayAttributeValue(MaxAttribute, Found);
		MaxAttributeValue->SetText(FText::AsNumber(CurrentMaxValue));
		if (FMath::IsNearlyEqual(CurrentMaxValue, 0.000000000001f))
		{
			AttributeProgressBar->SetPercent(0.f);
		}
		else
		{
			AttributeProgressBar->SetPercent(CurrentValue / CurrentMaxValue);
		}
		WeakASCCache->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UTheOneAttributeLine::OnMaxAttributeChanged);
	}
}

void UTheOneAttributeLine::UnBind()
{
	if (!Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}
	
	RemoveListeners(Tag2Attribute[AttributeTag]());
	WeakASCCache = nullptr;
}

void UTheOneAttributeLine::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue));
	if (bHasMaxAttribute)
	{
		bool Found = false;
		auto CurrentMaxValue = WeakASCCache->GetGameplayAttributeValue(Tag2Attribute[MaxAttributeTag](), Found);
		if (FMath::IsNearlyEqual(CurrentMaxValue, 0.000000000001f))
		{
			AttributeProgressBar->SetPercent(0.f);
		}
		else
		{
			AttributeProgressBar->SetPercent(OnAttributeChangeData.NewValue / CurrentMaxValue);
		}
	}
}

void UTheOneAttributeLine::OnMaxAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	check(bHasMaxAttribute);
	MaxAttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue));
	bool Found = false;
	auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(Tag2Attribute[AttributeTag](), Found);
	if (FMath::IsNearlyEqual(OnAttributeChangeData.NewValue, 0.000000000001f))
	{
		AttributeProgressBar->SetPercent(0.f);
	}
	else
	{
		AttributeProgressBar->SetPercent(CurrentValue / OnAttributeChangeData.NewValue);
	}
}
