// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/TheOneItemSystem.h"
#include "TheOnePlayerBag.generated.h"

class ATheOneCharacterBase;
class ACaptureActor;
class UTheOneAttributeLine;
class UTheOneGridSlot;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOnePlayerBag : public UUserWidget
{
	GENERATED_BODY()
	inline static FTransform DefaultCaptureTransform =
		FTransform(FRotator(0, 180, 0), FVector(120, 0, 30), FVector(1, 1, 1));
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> PropGridPanel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> WeaponGridPanel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> TeamGridPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> MainHandSlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> OffHandSlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> HeadSlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> ClothSlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> LeftJewelrySlot;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneGridSlot> RightJewelrySlot;

	// Todo: 角色背包对应的格子


	// Todo: 需要展示的属性的Line的绑定
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTheOneAttributeLine> HPAttributeLine;

	UPROPERTY()
	TWeakObjectPtr<ACaptureActor> CaptureActor;

	int32 CurrentSelectCharacterItemID = 0;
	TWeakObjectPtr<ATheOneCharacterBase> WeakCurrentSelectCharacter;

protected:
	void OnPostItemUpdated(int OldSlotID, const FTheOneItemInstance& InItemInstance);
	
	void OnSelectInBagCharacter(int32 InCharacterItemID);
	void UpdateEquipment(int32 InEquipmentID, UTheOneGridSlot* ToSlot) const;
};
