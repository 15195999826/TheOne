// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "TheOneCommonActionWidget.generated.h"

class SBox;
class SImage;
class UMaterialInstanceDynamic;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneCommonActionWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	// /** Begin UWidget */
	// virtual TSharedRef<SWidget> RebuildWidget() override;
	// virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// virtual void SynchronizeProperties() override;
	/** End UWidet */

// 	UFUNCTION(BlueprintCallable, Category = TheOneActionWidget)
// 	void SetEnhancedInputAction(UInputAction* InInputAction);
//
// 	UFUNCTION(BlueprintCallable, Category = TheOneActionWidget)
// 	void SetInputAction(FDataTableRowHandle InputActionRow);
//
// 	void SetInputActionBinding(FUIActionBindingHandle BindingHandle);
//
// 	UFUNCTION(BlueprintCallable, Category = TheOneActionWidget)
// 	void SetInputActions(TArray<FDataTableRowHandle> NewInputActions);
//
// 	UFUNCTION(BlueprintCallable, Category = TheOneActionWidget)
// 	void SetIconRimBrush(FSlateBrush InIconRimBrush);
//
// 	UFUNCTION(BlueprintCallable, Category = TheOneActionWidget)
// 	bool IsHeldAction() const;
//
// 	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputMethodChanged, bool, bUsingGamepad);
// 	UPROPERTY(BlueprintAssignable, Category = TheOneActionWidget)
// 	FOnInputMethodChanged OnInputMethodChanged;
//
// 	/**
// 	 * The material to use when showing held progress, the progress will be sent using the material parameter
// 	 * defined by ProgressMaterialParam and the value will range from 0..1.
// 	 **/
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TheOneActionWidget)
// 	FSlateBrush ProgressMaterialBrush;
//
// 	/** The material parameter on ProgressMaterialBrush to update the held percentage.  This value will be 0..1. */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheOneActionWidget)
// 	FName ProgressMaterialParam;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheOneActionWidget)
// 	FSlateBrush IconRimBrush;
//
// #if WITH_EDITOR
// 	virtual const FText GetPaletteCategory() override;
// #endif
//
// 	void OnActionProgress(float HeldPercent);
// 	void OnActionComplete();
// 	void SetProgressMaterial(const FSlateBrush& InProgressMaterialBrush, const FName& InProgressMaterialParam);
// 	void SetHidden(bool bAlwaysHidden);
//
// protected:
// 	/**
// 	 * List all the input actions that this common action widget is intended to represent.  In some cases you might have multiple actions
// 	 * that you need to represent as a single entry in the UI.  For example - zoom, might be mouse wheel up or down, but you just need to
// 	 * show a single icon for Up & Down on the mouse, this solves that problem.
// 	 */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheOneActionWidget, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase", TitleProperty = "RowName"))
// 	TArray<FDataTableRowHandle> InputActions;
//
// 	/**
// 	 * Input Action this common action widget is intended to represent. Optional if using EnhancedInputs
// 	 */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheOneActionWidget, meta = (EditCondition = "CommonInput.CommonInputSettings.IsEnhancedInputSupportEnabled", EditConditionHides))
// 	TObjectPtr<class UInputAction> EnhancedInputAction;
//
// #if WITH_EDITORONLY_DATA
// 	UPROPERTY()
// 	FDataTableRowHandle InputActionDataRow_DEPRECATED;
// #endif
//
// protected:
// 	const FCommonInputActionDataBase* GetInputActionData() const;
//
// 	void UpdateActionWidget();
//
// 	virtual void OnWidgetRebuilt() override;
// 	
// 	void UpdateBindingHandleInternal(FUIActionBindingHandle BindingHandle);
//
// 	void ListenToInputMethodChanged(bool bListen = true);
// 	void HandleInputMethodChanged(ECommonInputType InInputType);
//
// 	UPROPERTY(Transient)
// 	TObjectPtr<UMaterialInstanceDynamic> ProgressDynamicMaterial;
// 	
// 	TSharedPtr<SBox> MyKeyBox;
//
// 	TSharedPtr<SImage> MyIcon;
//
// 	TSharedPtr<SImage> MyProgressImage;
//
// 	TSharedPtr<SImage> MyIconRim;
//
// 	FSlateBrush Icon;
//
// 	bool bAlwaysHideOverride = false;
	
};
