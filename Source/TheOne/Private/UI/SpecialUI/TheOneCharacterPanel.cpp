// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneCharacterPanel.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "Character/TheOneCharacterBase.h"
#include "Components/ProgressBar.h"
#include "Game/TheOneGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/TheOneUIRoot.h"
#include "UI/SpecialUI/TheOneAbilityWidget.h"
#include "UI/SpecialUI/TheOneGridSlot.h"
#include "UI/SpecialUI/TheOneItemUserWidget.h"

void UTheOneCharacterPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	ItemSystem->OnPostItemUpdated.AddUObject(this, &UTheOneCharacterPanel::OnPostItemUpdated);
}

void UTheOneCharacterPanel::UpdateAbilitySlot()
{
	if (!WeakCacheCharacter.IsValid())
	{
		return;
	}
	
	AbilitySlotA->BindData(WeakCacheCharacter->WeakWeaponAbilityA.Get());
	AbilitySlotB->BindData(WeakCacheCharacter->WeakWeaponAbilityB.Get());
	AbilitySlotC->BindData(WeakCacheCharacter->WeakWeaponAbilityC.Get());
}

void UTheOneCharacterPanel::BindData_Implementation(ATheOneCharacterBase* InTheOneCharacterBase)
{
	// 移除旧的监听
	if (WeakCacheCharacter.IsValid())
	{
		if (WeakCacheCharacter == InTheOneCharacterBase)
		{
			return;
		}

		auto OldASC = WeakCacheCharacter->GetAbilitySystemComponent();
		auto OldAS = WeakCacheCharacter->GetLifeAttributeSet();
		OldASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetHealthAttribute()).RemoveAll(this);
		OldASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetMaxHealthAttribute()).RemoveAll(this);
		OldASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetManaAttribute()).RemoveAll(this);
		OldASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetMaxManaAttribute()).RemoveAll(this);
	}
	
	WeakCacheCharacter = InTheOneCharacterBase;
	auto ASC = InTheOneCharacterBase->GetAbilitySystemComponent();
	auto AS = InTheOneCharacterBase->GetLifeAttributeSet();
	UpdateHP(AS->GetHealth(), AS->GetMaxHealth());
	UpdateMana(AS->GetMana(), AS->GetMaxMana());
	// 设置HP、MP, 监听数据变化
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(this, &UTheOneCharacterPanel::OnHPChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddUObject(this, &UTheOneCharacterPanel::OnMaxHPChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute()).AddUObject(this, &UTheOneCharacterPanel::OnManaChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute()).AddUObject(this, &UTheOneCharacterPanel::OnMaxManaChanged);
	EmptyWeaponItem->SetVisibility(ESlateVisibility::Hidden);
	if (WeakCacheCharacter->WeaponID == INDEX_NONE)
	{
		auto DT = GetDefault<UTheOneDataTableSettings>()->WeaponTable.LoadSynchronous();
		auto DefaultRow = DT->FindRow<FTheOneWeaponConfig>(WeakCacheCharacter->DefaultWeaponRow, TEXT("WeaponConfig"));
		EmptyWeaponItem->SetupWeaponConfig(true, WeakCacheCharacter->DefaultWeaponRow, *DefaultRow);
		WeaponSlot->SetContent(EmptyWeaponItem);
	}
	else
	{
		// 显示当前武器
		UpdateWeapon(WeakCacheCharacter->WeaponID);
	}

	// 技能槽位显示对应的图标
	UpdateAbilitySlot();
}

void UTheOneCharacterPanel::UpdateWeapon(int32 InWeaponID)
{
	auto UIRoot = Cast<UTheOneGameInstance>(UGameplayStatics::GetGameInstance(this))->UIRoot;
	auto WeaponItemWidget = UIRoot->FindItemWidget(InWeaponID);
	check(WeaponItemWidget);
	WeaponSlot->SetContent(WeaponItemWidget);
}

void UTheOneCharacterPanel::OnPostItemUpdated(int32 OldSlotID, const FTheOneItemInstance& InItemInstance)
{
	if (!IsVisible())
	{
		return;
	}
	
	check(WeakCacheCharacter.IsValid());
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	// 检查Owner是不是当前角色
	const auto SlotInfo = ItemSystem->GetLogicSlotInfo(InItemInstance.LogicSlotID);
	if (SlotInfo->SlotType == ETheOneGridSlotType::CharacterWeapon && SlotInfo->OwnerFlag == WeakCacheCharacter->GetFlag())
	{
		UpdateWeapon(InItemInstance.ItemID);
		// Todo: 更新技能槽位
		// 技能槽位显示对应的图标
		UpdateAbilitySlot();
	}
}

void UTheOneCharacterPanel::OnHPChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	auto MaxHP = WeakCacheCharacter->GetLifeAttributeSet()->GetMaxHealth();
	UpdateHP(OnAttributeChangeData.NewValue, MaxHP);
}

void UTheOneCharacterPanel::OnMaxHPChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	auto HP = WeakCacheCharacter->GetLifeAttributeSet()->GetHealth();
	UpdateHP(HP, OnAttributeChangeData.NewValue);
}

void UTheOneCharacterPanel::OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	auto MaxMana = WeakCacheCharacter->GetLifeAttributeSet()->GetMaxMana();
	UpdateMana(OnAttributeChangeData.NewValue, MaxMana);
}

void UTheOneCharacterPanel::OnMaxManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	auto Mana = WeakCacheCharacter->GetLifeAttributeSet()->GetMana();
	UpdateMana(Mana, OnAttributeChangeData.NewValue);
}

void UTheOneCharacterPanel::UpdateHP(float InHP, float InMaxHP)
{
	// Todo:设置Timer 更新UI, 临时就直接设置值
	HPTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), InHP, InMaxHP)));
	HPProgressBar->SetPercent(InHP / InMaxHP);
	HPProgressBarGhost->SetPercent(InHP / InMaxHP);
}

void UTheOneCharacterPanel::UpdateMana(float InMana, float InMaxMana)
{
	MPTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), InMana, InMaxMana)));
	MPProgressBar->SetPercent(InMana / InMaxMana);
	MPProgressBarGhost->SetPercent(InMana / InMaxMana);
}
