// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneAbilityPanel.h"
#include "UI/SpecialUI/TheOneAbilityWidget.h"
#include "AbilitySystemComponent.h"
#include "Components/HorizontalBox.h"
#include "Game/TheOneEventSystem.h"

void UTheOneAbilityPanel::UnBind()
{
	// 遍历Child
	auto Children = AbilityWidgetContainer->GetAllChildren();
	for (auto Child : Children)
	{
		auto AbilityWidget = CastChecked<UTheOneAbilityWidget>(Child);
		ReleaseAbilityWidget(AbilityWidget);
	}
}

void UTheOneAbilityPanel::Bind(ATheOneCharacterBase* InCharacter)
{
	const auto& AbilityCaches = InCharacter->AbilityCaches;
	int Count = 0;
	for (const auto& AbilityCache : AbilityCaches)
	{
		for (const auto& Ability : AbilityCache.Value)
		{
			if (!Ability.AbilityGA.IsValid())
			{
				continue;
			}
			auto AbilityWidget = GetAbilityWidget();
			AbilityWidget->BindAbilityData(Ability.AbilityGA.Get(), Count);
			AbilityWidgetContainer->AddChild(AbilityWidget);

			AbilityWidget->OnExecuteAbility.AddDynamic(this, &UTheOneAbilityPanel::OnExecuteAbility);
			
			Count++;
		}
	}
	
	// Todo: 监听角色技能的变化, 可以做成监听角色装备的变化，技能只会来源于装备
}

UTheOneAbilityWidget* UTheOneAbilityPanel::GetAbilityWidget()
{
	if (AbilityWidgetPool.Num() > 0)
	{
		auto Widget = AbilityWidgetPool.Pop();
		return Widget;
	}

	return CreateWidget<UTheOneAbilityWidget>(GetWorld(), AbilityWidgetClass);
}

void UTheOneAbilityPanel::ReleaseAbilityWidget(UTheOneAbilityWidget* InWidget)
{
	InWidget->OnExecuteAbility.RemoveAll(this);
	InWidget->UnBindAbilityData();
	InWidget->RemoveFromParent();
	AbilityWidgetPool.Add(InWidget);
}

void UTheOneAbilityPanel::OnExecuteAbility(int InIntPayLoad)
{
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	FTheOneUseAbilityCommandPayload Payload;
	Payload.CommandType = ETheOneUseAbilityCommandType::UseAbility;
	Payload.IntPayload = InIntPayLoad;
	EventSystem->UseAbilityCommand.Broadcast(Payload);
}
