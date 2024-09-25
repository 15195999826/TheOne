// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "UI/TheOneUserWidget.h"
#include "UI/TheOneViewController.h"
#include "BattleWindowController.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UBattleWindowController : public UTheOneViewController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Setup(UProgressBar* HPBar, UTextBlock* HPBarText,
		UProgressBar* ManaBar, UTextBlock* ManaBarText,
		UTheOneUserWidget* SkillSlot0,
		UTheOneUserWidget* SkillSlot1,
		UTheOneUserWidget* SkillSlot2,
		UTheOneUserWidget* InBag,
		UButton* OpenBagButton,
		UHorizontalBox* InShieldPanel,
		UTextBlock* InShieldText);
	
	
	UFUNCTION(BlueprintCallable)
	void SetupContextMenu(USizeBox* InContextMenuRoot, UButton* EquipBtn, UButton* SalvageBtn, UButton* DropBtn, UButton* SplitBtn);
};
