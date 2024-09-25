#include "TheOneAbilityRowCustomLayout.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Development/TheOneDataTableSettings.h"

void FTheOneAbilityRowCustomLayout::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                        FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// 不绘制默认的Header
}

void FTheOneAbilityRowCustomLayout::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                         IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	RowNameHandle = InStructPropertyHandle->GetChildHandle("RowName");
	if (RowNameHandle.IsValid() && RowNameHandle->IsValidHandle())
	{
		// 获取InStructPropertyHandle的名字
		FName StructName = InStructPropertyHandle->GetProperty()->GetFName();
		
		// 绘制一行输入框和按钮, 输入框从ComboBox中选择，按钮点击后添加元素到AbilitiesHandle这个数组中
		FPropertyComboBoxArgs ComboArgs(RowNameHandle,
										FOnGetPropertyComboBoxStrings::CreateSP(
											this, &FTheOneAbilityRowCustomLayout::OnGetRowStrings),
										FOnGetPropertyComboBoxValue::CreateSP(
											this, &FTheOneAbilityRowCustomLayout::OnGetRowNameValueString));
		ComboArgs.ShowSearchForItemCount = 1;

		StructBuilder.AddCustomRow(FText::FromString(""))
					 .RowTag("AbilitySelector")
					 .NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString(StructName.ToString()))
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
			];
	}
}

void FTheOneAbilityRowCustomLayout::OnGetRowStrings(TArray<TSharedPtr<FString>>& OutStrings,
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

FString FTheOneAbilityRowCustomLayout::OnGetRowNameValueString() const
{
	if (!RowNameHandle.IsValid() || !RowNameHandle->IsValidHandle())
	{
		return FString();
	}

	// Todo: 这里可以做本地化操作，当获得了当前的值，可以根据值返回对应的本地化字符串
	FName RowNameValue;
	const FPropertyAccess::Result RowResult = RowNameHandle->GetValue(RowNameValue);
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