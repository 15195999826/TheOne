// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Interface/TheOneShorcutInterface.h"
#include "Item/TheOneItemSystem.h"
#include "TheOneCharacterPanel.generated.h"

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

protected:
	virtual void NativeOnInitialized() override;
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> WeaponSlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneItemUserWidget> EmptyWeaponItem;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAbilityWidget> AbilitySlotA;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAbilityWidget> AbilitySlotB;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAbilityWidget> AbilitySlotC;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot3;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot4;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot5;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> PropSlot6;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HPTextBlock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPProgressBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPProgressBarGhost;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MPTextBlock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> MPProgressBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> MPProgressBarGhost;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ATheOneCharacterBase> WeakCacheCharacter;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindData(ATheOneCharacterBase* InTheOneCharacterBase);

protected:
	
private:
	void UpdateAbilitySlot();
	void UpdateWeapon(int32 InWeaponID);
	void OnPostItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance);
	
	void OnHPChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHPChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxManaChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void UpdateHP(float InHP, float InMaxHP);
	void UpdateMana(float InMana, float InMaxMana);
};
