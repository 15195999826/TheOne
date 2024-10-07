// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "TheOneAttributeLine.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneAttributeLine : public UUserWidget
{
	GENERATED_BODY()

	static TMap<FGameplayTag, TFunction<FGameplayAttribute()>> Tag2Attribute;
	
protected:
	virtual void NativePreConstruct() override;
	void RemoveListeners(const FGameplayAttribute& Attribute);

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	FGameplayTag AttributeTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	FGameplayTag MaxAttributeTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	bool bShowProgressBar;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeName;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeValue;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeSpace;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxAttributeValue;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProgressBar> AttributeProgressBar;

	
	
protected:
	TWeakObjectPtr<UAbilitySystemComponent> WeakASCCache;
	
public:
	void Bind(UAbilitySystemComponent* InASC);
	void UnBind();

private:
	bool bHasMaxAttribute = false;
	void OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);
};
