// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAttributeLine.h"

#include "TheOneGameplayTags.h"
#include "AbilitySystem/TheOneAbilitySystemComponent.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// 初始化Tag2Attribute
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
	
	if (TheOneGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
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
		if (TheOneGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
		{
			const auto& MaxAttribute =TheOneGameplayTags::Tag2Attribute[MaxAttributeTag]();
			WeakASCCache->GetGameplayAttributeValueChangeDelegate(MaxAttribute).RemoveAll(this);
		}
	}
}

void UTheOneAttributeLine::Bind(UAbilitySystemComponent* InASC)
{
	if (!TheOneGameplayTags::Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}

	const FGameplayAttribute& Attribute = TheOneGameplayTags::Tag2Attribute[AttributeTag]();
	
	RemoveListeners(Attribute);
	
	WeakASCCache = InASC;

	// 直接进行一次更新
	bool Found = false;
	auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(Attribute, Found);
	AttributeValue->SetText(FText::AsNumber(CurrentValue));

	WeakASCCache->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UTheOneAttributeLine::OnAttributeChanged);
	if (TheOneGameplayTags::Tag2Attribute.Contains(MaxAttributeTag))
	{
		bHasMaxAttribute = true;
		const auto& MaxAttribute = TheOneGameplayTags::Tag2Attribute[MaxAttributeTag]();
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
	if (!TheOneGameplayTags::Tag2Attribute.Contains(AttributeTag))
	{
		return;
	}
	
	RemoveListeners(TheOneGameplayTags::Tag2Attribute[AttributeTag]());
	WeakASCCache = nullptr;
}

void UTheOneAttributeLine::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	AttributeValue->SetText(FText::AsNumber(OnAttributeChangeData.NewValue));
	if (bHasMaxAttribute)
	{
		bool Found = false;
		auto CurrentMaxValue = WeakASCCache->GetGameplayAttributeValue(TheOneGameplayTags::Tag2Attribute[MaxAttributeTag](), Found);
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
	auto CurrentValue = WeakASCCache->GetGameplayAttributeValue(TheOneGameplayTags::Tag2Attribute[AttributeTag](), Found);
	if (FMath::IsNearlyEqual(OnAttributeChangeData.NewValue, 0.000000000001f))
	{
		AttributeProgressBar->SetPercent(0.f);
	}
	else
	{
		AttributeProgressBar->SetPercent(CurrentValue / OnAttributeChangeData.NewValue);
	}
}
