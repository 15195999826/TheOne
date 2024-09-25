// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneMainWindow.h"

#include "Components/UniformGridPanel.h"
#include "Game/TheOneEventSystem.h"
#include "Game/TheOneGameModeBase.h"
#include "UI/SpecialUI/TheOneGridSlot.h"
#include "Interface/TheOneShorcutInterface.h"
#include "Item/TheOneItemSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SpecialUI/TheOneAbilityWidget.h"
#include "UI/SpecialUI/TheOneCharacterPanel.h"
#include "UI/SpecialUI/TheOnePlayerBag.h"

void UTheOneMainWindow::InitMainWindow()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnSelectCharacter.AddUObject(this, &UTheOneMainWindow::OnSelectCharacter);
	EventSystem->OnDeSelectCharacter.AddUObject(this, &UTheOneMainWindow::OnDeSelectCharacter);

	auto TheOneGM = Cast<ATheOneGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (TheOneGM && TheOneGM->EnablePlayerControl)
	{
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->AbilitySlotA);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->AbilitySlotB);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->AbilitySlotC);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot1);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot2);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot3);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot4);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot5);
		ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->PropSlot6);
	}
}

void UTheOneMainWindow::OnSelectCharacter(ATheOneCharacterBase* InTheOneCharacterBase)
{
	CharacterPanel->BindData(InTheOneCharacterBase);
	CharacterPanel->SetVisibility(ESlateVisibility::Visible);
}

void UTheOneMainWindow::OnDeSelectCharacter()
{
	CharacterPanel->SetVisibility(ESlateVisibility::Hidden);
}