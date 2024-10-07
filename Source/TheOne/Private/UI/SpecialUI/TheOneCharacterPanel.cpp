// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneCharacterPanel.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/ProgressBar.h"
#include "Game/TheOneGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneTeamSystem.h"
#include "UI/TheOneUIRoot.h"
#include "UI/SpecialUI/TheOneAbilityWidget.h"
#include "UI/SpecialUI/TheOneAttributeLine.h"
#include "UI/SpecialUI/TheOneGridSlot.h"
#include "UI/SpecialUI/TheOneItemUserWidget.h"


void UTheOneCharacterPanel::UnBind()
{
	ActionPointLine->UnBind();
	EnergyLine->UnBind();
	WillLine->UnBind();
	HeadArmorLine->UnBind();
	BodyArmorLine->UnBind();
	HPLine->UnBind();
	NameTextBlock->SetText(FText::FromString(""));
}


void UTheOneCharacterPanel::Bind_Implementation(ATheOneCharacterBase* InTheOneCharacterBase)
{
	auto TeamSystem = GetWorld()->GetSubsystem<UTheOneTeamSystem>();
	auto Unique = TeamSystem->GetCharacterUnique(InTheOneCharacterBase->GetFlag());
	
	WeakCacheCharacter = InTheOneCharacterBase;
	auto ASC = InTheOneCharacterBase->GetAbilitySystemComponent();
	ActionPointLine->Bind(ASC);
	EnergyLine->Bind(ASC);
	WillLine->Bind(ASC);
	HeadArmorLine->Bind(ASC);
	BodyArmorLine->Bind(ASC);
	HPLine->Bind(ASC);
	NameTextBlock->SetText(Unique.Name);
}
