// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneGridSlot.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/NamedSlot.h"
#include "Game/TheOneGameInstance.h"
#include "Item/TheOneItemSystem.h"
#include "UI/TheOneUIRoot.h"


void UTheOneGridSlot::NativePreConstruct()
{
	if (!IsDesignTime())
	{
		auto TheOneGameInstance = GetWorld()->GetGameInstance<UTheOneGameInstance>();
		TheOneGameInstance->UIRoot->RegisterGridSlot(ID, this);
	}
	Super::NativePreConstruct();
}

bool UTheOneGridSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                 UDragDropOperation* InOperation)
{
	auto ItemVC = Cast<UTheOneItemUserWidget>(InOperation->Payload);
	if(CanDropOnSlot(ItemVC))
	{
		auto TheOneGameInstance = GetWorld()->GetGameInstance<UTheOneGameInstance>();
		TheOneGameInstance->UIRoot->DropItemOnGridSlot(ID, OwnerType, ItemVC);
	}
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UTheOneGridSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UTheOneGridSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UTheOneGridSlot::SetContent(UTheOneItemUserWidget* ItemWidget)
{
	DropOnSlot->RemoveChildAt(0);
	
	ItemWidget->SetVisibility(ESlateVisibility::Visible);
	DropOnSlot->AddChild(ItemWidget);
}



