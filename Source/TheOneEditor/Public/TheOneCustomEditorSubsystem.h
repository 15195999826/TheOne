// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "TheOneCustomEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class THEONEEDITOR_API UTheOneCustomEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	TSharedPtr<FExtender> MenuExtender;

	
	FDelegateHandle OnGetGlobalRowExtensionHandle;

	void OnExtendLevelEditorMenu(FMenuBuilder& MenuBuilder);
	void RegisterDetailPanelCustomization();

	/** Removes details panel button customization */
	void UnregisterDetailPanelCustomization();

	void OnGetGlobalRowExtension(const FOnGenerateGlobalRowExtensionArgs& OnGenerateGlobalRowExtensionArgs, TArray<FPropertyRowExtensionButton>& OutExtensions);
	TSharedPtr<SWidget> GetExposeButton(const FOnGenerateGlobalRowExtensionArgs& InArgs,FString TableName);
	void JumpToTable(const FString& TableName, int32 ID);

};
