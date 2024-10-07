// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneBattleWindow.h"

#include "Game/TheOneEventSystem.h"
#include "UI/SpecialUI/TheOneAbilityPanel.h"
#include "UI/SpecialUI/TheOneCharacterPanel.h"

void UTheOneBattleWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UTheOneBattleWindow::OnActive_Implementation()
{
	// ITheOneShorcutInterface::Execute_BindShortcutKey(CharacterPanel->AbilitySlotA);
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterGetTurn.AddUObject(this, &UTheOneBattleWindow::OnCharacterGetTurn);
	EventSystem->OnCharacterEndTurn.AddUObject(this, &UTheOneBattleWindow::OnCharacterEndTurn);
}

void UTheOneBattleWindow::OnDeActive_Implementation()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterGetTurn.RemoveAll(this);
}

void UTheOneBattleWindow::OnCharacterGetTurn(ATheOneCharacterBase* InCharacter)
{
	CharacterPanel->Bind(InCharacter);
	AbilityPanel->Bind(InCharacter);
}

void UTheOneBattleWindow::OnCharacterEndTurn(ATheOneCharacterBase* InCharacter)
{
	CharacterPanel->UnBind();
	AbilityPanel->UnBind();
}
