// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/TheOneCommonActionWidget.h"

UTheOneCommonActionWidget::UTheOneCommonActionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}
//
// TSharedRef<SWidget> UTheOneCommonActionWidget::RebuildWidget()
// {
// 	if (!IsDesignTime() && ProgressDynamicMaterial == nullptr)
// 	{
// 		UMaterialInstanceDynamic* const ParentMaterialDynamic = Cast<UMaterialInstanceDynamic>(ProgressMaterialBrush.GetResourceObject());
// 		if (ParentMaterialDynamic == nullptr)
// 		{
// 			UMaterialInterface* ParentMaterial = Cast<UMaterialInterface>(ProgressMaterialBrush.GetResourceObject());
// 			if (ParentMaterial)
// 			{
// 				ProgressDynamicMaterial = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
// 				ProgressMaterialBrush.SetResourceObject(ProgressDynamicMaterial);
// 			}
// 			else
// 			{
// 				ProgressDynamicMaterial = nullptr;
// 			}
// 		}
// 	}
//
// 	MyKeyBox = SNew(SBox)
// 		.HAlign(HAlign_Center)
// 		.VAlign(VAlign_Center);
//
// 	MyKeyBox->SetContent(	
// 		SNew(SOverlay)
// 		+ SOverlay::Slot()
// 		.HAlign(HAlign_Center)
// 		.VAlign(VAlign_Center)
// 		[
// 			SAssignNew(MyIconRim, SImage)
// 			.Image(&IconRimBrush)
// 		]
// 		+ SOverlay::Slot()
// 		.HAlign(HAlign_Center)
// 		.VAlign(VAlign_Center)
// 		[
// 			SAssignNew(MyProgressImage, SImage)
// 			.Image(&ProgressMaterialBrush)
// 		]
// 		+ SOverlay::Slot()
// 		.HAlign(HAlign_Center)
// 		.VAlign(VAlign_Center)
// 		[
// 			SAssignNew(MyIcon, SImage)
// 			.Image(&Icon)
// 		]);
// 	
// 	return MyKeyBox.ToSharedRef();
// }
//
// void UTheOneCommonActionWidget::ReleaseSlateResources(bool bReleaseChildren)
// {
// 	MyProgressImage.Reset();
// 	MyIcon.Reset();
// 	MyKeyBox.Reset();
// 	
// 	ListenToInputMethodChanged(false);
// 	Super::ReleaseSlateResources(bReleaseChildren);
// }
//
// void UTheOneCommonActionWidget::SynchronizeProperties()
// {
// 	Super::SynchronizeProperties();
//
// 	if (MyKeyBox.IsValid() && IsDesignTime())
// 	{
// 		UpdateActionWidget();
// 	}
// }
