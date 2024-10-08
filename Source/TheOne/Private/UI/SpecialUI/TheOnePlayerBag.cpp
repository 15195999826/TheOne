// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOnePlayerBag.h"

#include "CaptureActor.h"
#include "EngineUtils.h"
#include "AI/TheOneAIController.h"
#include "Game/TheOneEventSystem.h"
#include "Game/TheOneGameInstance.h"
#include "Game/TheOneGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/TheOneContextSystem.h"
#include "Subsystems/TheOneTeamSystem.h"
#include "UI/TheOneUIRoot.h"
#include "UI/SpecialUI/TheOneAttributeLine.h"
#include "UI/SpecialUI/TheOneGridSlot.h"

void UTheOnePlayerBag::NativeConstruct()
{
	Super::NativeConstruct();

	for (TActorIterator<ACaptureActor> It(GetWorld()); It; ++It)
	{
		CaptureActor = *It;
		break;
	}
	
	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->OnSelectInBagCharacter.AddUObject(this, &UTheOnePlayerBag::OnSelectInBagCharacter);
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	ItemSystem->OnPostItemUpdated.AddUObject(this, &UTheOnePlayerBag::OnPostItemUpdated);
}

void UTheOnePlayerBag::OnPostItemUpdated(int OldSlotID, const FTheOneItemInstance& InItemInstance)
{
	if (!WeakCurrentSelectCharacter.IsValid())
	{
		return;
	}
	
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	// 检查Owner是不是当前角色
	const auto SlotInfo = ItemSystem->GetLogicSlotInfo(InItemInstance.LogicSlotID);
	if (SlotInfo->OwnerFlag == WeakCurrentSelectCharacter->GetFlag() && SlotInfo->SlotType == ETheOneGridSlotType::CharacterBag)
	{
		auto SlotType = ItemSystem->SlotID2CharacterBagSlotType(SlotInfo->OwnerFlag, InItemInstance.LogicSlotID);
		switch (SlotType) {
			case ETheOneCharacterBagSlotType::MainHand:
				UpdateEquipment(InItemInstance.ItemID, MainHandSlot);
				break;
			case ETheOneCharacterBagSlotType::OffHand:
				UpdateEquipment(InItemInstance.ItemID, OffHandSlot);
				break;
			case ETheOneCharacterBagSlotType::Head:
				UpdateEquipment(InItemInstance.ItemID, HeadSlot);
				break;
			case ETheOneCharacterBagSlotType::Cloth:
				UpdateEquipment(InItemInstance.ItemID, ClothSlot);
				break;
			case ETheOneCharacterBagSlotType::LeftJewelry:
				UpdateEquipment(InItemInstance.ItemID, LeftJewelrySlot);
				break;
			case ETheOneCharacterBagSlotType::RightJewelry:
				UpdateEquipment(InItemInstance.ItemID, RightJewelrySlot);
				break;
			case ETheOneCharacterBagSlotType::Store:
				break;
		}
		
		
	}
}

void UTheOnePlayerBag::OnSelectInBagCharacter(int32 InCharacterItemID)
{
	if (CurrentSelectCharacterItemID == InCharacterItemID)
	{
		return;
	}

	if (InCharacterItemID == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("OnSelectInBagCharacter INDEX_NONE"));
		return;
	}
	
	CurrentSelectCharacterItemID = InCharacterItemID;
	auto TeamSystem = GetWorld()->GetSubsystem<UTheOneTeamSystem>();
	const auto& CharacterUnique = TeamSystem->GetCharacterUniqueByHookedItemID(InCharacterItemID);
	auto GameMode = GetWorld()->GetAuthGameMode<ATheOneGameModeBase>();
	check(GameMode->SpawnedAIMap.Contains(CharacterUnique.Flag))
	auto Pawn = GameMode->SpawnedAIMap[CharacterUnique.Flag]->GetPawn();
	WeakCurrentSelectCharacter = GetWorld()->GetSubsystem<UTheOneContextSystem>()->WeakBagSelectCharacter = Cast<ATheOneCharacterBase>(Pawn);
	// 角色对应的Panel的更新
	if (CaptureActor.IsValid())
	{
		CaptureActor->CaptureActorOnly(Pawn, DefaultCaptureTransform);
	}

	// Todo: 清除旧的属性监听， 并绑定到新角色
	SpeedLine->Bind(WeakCurrentSelectCharacter->GetAbilitySystemComponent());
	
	// 显示当前装备
	UpdateEquipment(WeakCurrentSelectCharacter->MainHandItemID, MainHandSlot);
	UpdateEquipment(WeakCurrentSelectCharacter->OffHandItemID, OffHandSlot);
	UpdateEquipment(WeakCurrentSelectCharacter->HeadItemID, HeadSlot);
	UpdateEquipment(WeakCurrentSelectCharacter->ClothItemID, ClothSlot);
	UpdateEquipment(WeakCurrentSelectCharacter->LeftJewelryItemID, LeftJewelrySlot);
	UpdateEquipment(WeakCurrentSelectCharacter->RightJewelryItemID, RightJewelrySlot);
}

void UTheOnePlayerBag::UpdateEquipment(int32 InEquipmentID, UTheOneGridSlot* ToSlot) const
{
	if (InEquipmentID == INDEX_NONE)
	{
		ToSlot->SetContent(nullptr);
	}
	else
	{
		auto UIRoot = Cast<UTheOneGameInstance>(UGameplayStatics::GetGameInstance(this))->UIRoot;
		auto WeaponItemWidget = UIRoot->FindItemWidget(InEquipmentID);
		check(WeaponItemWidget);
		ToSlot->SetContent(WeaponItemWidget);
	}
	
}

