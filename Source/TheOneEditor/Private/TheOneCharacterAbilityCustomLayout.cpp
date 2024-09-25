#include "TheOneCharacterAbilityCustomLayout.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Development/TheOneDataTableSettings.h"

void FTheOneCharacterAbilityCustomLayout::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                        FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// 绘制默认的Header
	HeaderRow.NameContent()
	[
		InStructPropertyHandle->CreatePropertyNameWidget()
	];
}

void FTheOneCharacterAbilityCustomLayout::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                         IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// NormalAbilityHandle = InStructPropertyHandle->GetChildHandle("NormalAbilityRow");
	AbilityRowSelectorHandle = InStructPropertyHandle->GetChildHandle("AbilitySelector");
	AbilitiesHandle = InStructPropertyHandle->GetChildHandle("AbilityRows");

	// if (NormalAbilityHandle.IsValid() && NormalAbilityHandle->IsValidHandle())
	// {
	// 	// 绘制一个下拉菜单，可以选择“Test1”、“Test2”、“Test3”
	// 	FPropertyComboBoxArgs ComboArgs(NormalAbilityHandle,
	// 	                                FOnGetPropertyComboBoxStrings::CreateSP(
	// 		                                this, &FTheOneCharacterAbilityCustomLayout::OnGetRowStrings),
	// 	                                FOnGetPropertyComboBoxValue::CreateSP(
	// 		                                this, &FTheOneCharacterAbilityCustomLayout::OnGetNormalAbilityValueString));
	// 	ComboArgs.ShowSearchForItemCount = 1;
	//
	// 	StructBuilder.AddCustomRow(FText::FromString(""))
	// 	             .RowTag("NormalAbility")
	// 	             .NameContent()
	// 		[
	// 			SNew(STextBlock)
	// 			.Text(FText::FromString("Normal Ability"))
	// 			.Font(StructCustomizationUtils.GetRegularFont())
	// 		]
	// 		.ValueContent()
	// 		.MaxDesiredWidth(0.0f) // don't constrain the combo button width
	// 		[
	// 			PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs)
	// 		];
	// }
	
	if (AbilityRowSelectorHandle.IsValid() && AbilityRowSelectorHandle->IsValidHandle())
	{
		// 绘制一行输入框和按钮, 输入框从ComboBox中选择，按钮点击后添加元素到AbilitiesHandle这个数组中
		FPropertyComboBoxArgs ComboArgs(AbilityRowSelectorHandle,
										FOnGetPropertyComboBoxStrings::CreateSP(
											this, &FTheOneCharacterAbilityCustomLayout::OnGetRowStrings),
										FOnGetPropertyComboBoxValue::CreateSP(
											this, &FTheOneCharacterAbilityCustomLayout::OnGetAbilitySelectorValueString));
		ComboArgs.ShowSearchForItemCount = 1;

		StructBuilder.AddCustomRow(FText::FromString(""))
					 .RowTag("AbilitySelector")
					 .NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Ability Selector"))
				.Font(StructCustomizationUtils.GetRegularFont())
			]
			.ValueContent()
			.MaxDesiredWidth(0.0f) // don't constrain the combo button width
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString("Add"))
					.OnClicked(FOnClicked::CreateSP(this, &FTheOneCharacterAbilityCustomLayout::OnAddNewAbilityToArray))
				]
			];
	}


	if (AbilitiesHandle.IsValid())
	{
		StructBuilder.AddProperty(AbilitiesHandle.ToSharedRef());
	}
	
	
	// if (TestCustomHandle.IsValid() && TestCustomHandle->IsValidHandle())
	// {
	// 	// 绘制一个下拉菜单，可以选择“Test1”、“Test2”、“Test3”
	// 	FPropertyComboBoxArgs ComboArgs(TestCustomHandle, 
	// 	FOnGetPropertyComboBoxStrings::CreateSP(this, &FTheOneCharacterConfigCustomLayout::OnGetRowStrings), 
	// 	FOnGetPropertyComboBoxValue::CreateSP(this, &FTheOneCharacterConfigCustomLayout::OnGetRowValueString));
	// 	ComboArgs.ShowSearchForItemCount = 1;
	//
	// 	StructBuilder.AddCustomRow(FText::FromString("Test Custom"))
	// 		.RowTag("Test")
	// 		.NameContent()
	// 		[
	// 			SNew(STextBlock)
	// 			.Text(FText::FromString("Test Custom2"))
	// 			.Font(StructCustomizationUtils.GetRegularFont())
	// 		]
	// 		.ValueContent()
	// 		.MaxDesiredWidth(0.0f) // don't constrain the combo button width
	// 		[
	// 			PropertyCustomizationHelpers::MakePropertyComboBox(ComboArgs)
	// 		];
	// }


	// 功能保留, 直接放入默认的子属性， 将会绘制默认的子属性
	uint32 NumChildren;
	InStructPropertyHandle->GetNumChildren(NumChildren);
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = InStructPropertyHandle->GetChildHandle(ChildIndex);
		if (ChildHandle.IsValid() && ChildHandle->GetProperty()->GetName() != "AbilityRows" && ChildHandle->GetProperty()->GetName() != "AbilitySelector")
		{
			StructBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}


	// 功能保留，添加Button， 点击后弹出下拉菜单
	// if (TestCustomHandle.IsValid() && TestCustomHandle->IsValidHandle())
	// {
	// 	StructBuilder.AddCustomRow(FText::FromString("Test Custom"))
	// 		.NameContent()
	// 		[
	// 			SNew(STextBlock)
	// 			.Text(FText::FromString("Test Custom"))
	// 			.Font(StructCustomizationUtils.GetRegularFont())
	// 		]
	// 		.ValueContent()
	// 		.MaxDesiredWidth(0.0f)
	// 		[
	// 			SNew(SButton)
	// 			.Text(FText::FromString("Select"))
	// 			.OnClicked(FOnClicked::CreateSP(this, &FTheOneCharacterAbilityCustomLayout::OnSelectButtonClicked))
	// 		];
	// }
}

void FTheOneCharacterAbilityCustomLayout::OnGetRowStrings(TArray<TSharedPtr<FString>>& OutStrings,
	TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems)
{
	TArray<FName> AllRowNames{ NAME_None };

	auto SoftDT = GetDefault<UTheOneDataTableSettings>()->AbilityTable.LoadSynchronous();
	if (SoftDT)
	{
		auto RowNames = SoftDT->GetRowNames();
		for (auto RowName : RowNames)
		{
			AllRowNames.Add(RowName);
		}
	}
	
	for (const FName& RowName : AllRowNames)
	{
		OutStrings.Add(MakeShared<FString>(RowName.ToString()));
		OutRestrictedItems.Add(false);
	}
}

FString FTheOneCharacterAbilityCustomLayout::OnGetAbilitySelectorValueString() const
{
	return InternalOnGetRowValueString(AbilityRowSelectorHandle);
}

FReply FTheOneCharacterAbilityCustomLayout::OnAddNewAbilityToArray()
{
	FName RowNameValue;
	const FPropertyAccess::Result RowResult = AbilityRowSelectorHandle->GetValue(RowNameValue);
	if (RowResult == FPropertyAccess::Success && !RowNameValue.IsNone())
	{
		// Get the array handle
		TSharedPtr<IPropertyHandleArray> ArrayHandle = AbilitiesHandle->AsArray();
		if (ArrayHandle.IsValid())
		{
			// Add a new element to the array
			ArrayHandle->AddItem();
            
			// Get the handle of the newly added element
			uint32 LastIndex;
			ArrayHandle->GetNumElements(LastIndex);
			LastIndex -= 1;
			TSharedPtr<IPropertyHandle> NewElementHandle = ArrayHandle->GetElement(LastIndex);
            
			// Set the value of the new element
			NewElementHandle->SetValue(RowNameValue);
		}
	}
	
	
	return FReply::Handled();
}

FString FTheOneCharacterAbilityCustomLayout::InternalOnGetRowValueString(TSharedPtr<IPropertyHandle> InHandle)
{
	if (!InHandle.IsValid() || !InHandle->IsValidHandle())
	{
		return FString();
	}

	// Todo: 这里可以做本地化操作，当获得了当前的值，可以根据值返回对应的本地化字符串
	FName RowNameValue;
	const FPropertyAccess::Result RowResult = InHandle->GetValue(RowNameValue);
	if (RowResult == FPropertyAccess::Success)
	{
		if (RowNameValue.IsNone())
		{
			return FString("None");
		}
		return RowNameValue.ToString();
	}
	else if (RowResult == FPropertyAccess::Fail)
	{
		return FString("Fail");
	}
	else
	{
		return FString("Multiple Values");
	}
}

void FTheOneCharacterAbilityCustomLayout::OnMenuItemSelected(FName Name)
{
	// if (!TestCustomHandle.IsValid() || !TestCustomHandle->IsValidHandle())
	// {
	// 	return;
	// }
	//
	// TestCustomHandle->SetValue(Name);
}

FReply FTheOneCharacterAbilityCustomLayout::OnSelectButtonClicked()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Test1"),
		FText::FromString("Select Test1"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &FTheOneCharacterAbilityCustomLayout::OnMenuItemSelected, FName("Test1")))
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Test2"),
		FText::FromString("Select Test2"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &FTheOneCharacterAbilityCustomLayout::OnMenuItemSelected, FName("Test2")))
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Test3"),
		FText::FromString("Select Test3"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &FTheOneCharacterAbilityCustomLayout::OnMenuItemSelected, FName("Test3")))
	);

	TSharedRef<SWidget> MenuWidget = MenuBuilder.MakeWidget();

	FSlateApplication::Get().PushMenu(
		FSlateApplication::Get().GetActiveTopLevelWindow().ToSharedRef(),
		FWidgetPath(),
		MenuWidget,
		FSlateApplication::Get().GetCursorPos(),
		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
	);

	return FReply::Handled();
}