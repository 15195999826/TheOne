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
	EventSystem->OnCharacterEndTurn.AddDynamic(this, &UTheOneBattleWindow::OnCharacterEndTurn);
}

void UTheOneBattleWindow::OnDeActive_Implementation()
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnCharacterGetTurn.RemoveAll(this);
}

void UTheOneBattleWindow::OnCharacterGetTurn(ATheOneCharacterBase* InCharacter)
{
	CharacterPanel->Bind(InCharacter);
	if (IInHexActorInterface::Execute_GetCamp(InCharacter) == ETheOneCamp::Player)
	{
		TempEndTurn->SetVisibility(ESlateVisibility::Visible);
		AbilityPanel->Bind(InCharacter);
	}
	else
	{
		TempEndTurn->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTheOneBattleWindow::OnCharacterEndTurn(ATheOneCharacterBase* InCharacter)
{
	CharacterPanel->UnBind();
	AbilityPanel->UnBind();
	TempEndTurn->SetVisibility(ESlateVisibility::Hidden);
}
