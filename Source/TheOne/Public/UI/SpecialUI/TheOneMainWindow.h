// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TheOneActivableWindowInterface.h"
#include "TheOneMainWindow.generated.h"

class ATheOneCharacterBase;
class UTheOneShop;
class UTheOnePlayerBag;
class UTheOneCharacterPanel;
/**
 * Todo: 打开背包时，如果当前没有选中的背包角色， 则选择最前面的那个
 */
UCLASS()
class THEONE_API UTheOneMainWindow : public UUserWidget, public ITheOneActivableWindowInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOnePlayerBag> PlayerBag;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneShop> Shop;

	// ITheOneActivableWindowInterface Start
public:
	virtual void OnActive_Implementation() override;
	virtual void OnDeActive_Implementation() override;
	
	// ITheOneActivableWindowInterface End

protected:
	void OnSelectCharacter(ATheOneCharacterBase* InTheOneCharacterBase);
	void OnDeSelectCharacter();
};
