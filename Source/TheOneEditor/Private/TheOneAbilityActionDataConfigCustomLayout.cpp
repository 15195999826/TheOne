#include "TheOneAbilityActionDataConfigCustomLayout.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneGameplayTags.h"
#include "Development/TheOneDataTableSettings.h"
#include "Types/TheOneProjectileConfig.h"

void FTheOneAbilityActionDataConfigCustomLayout::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                         FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// 绘制变量名，和一个按钮用于设置默认值
	HeaderRow.NameContent()
	[
		InStructPropertyHandle->CreatePropertyNameWidget()
	];
}

void FTheOneAbilityActionDataConfigCustomLayout::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                                         IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	ActionTypeHandle = InStructPropertyHandle->GetChildHandle("ActionType");
	CustomKVsHandle = InStructPropertyHandle->GetChildHandle("CustomKVs");
	// 监听ActionTypeHandle的变化，根据不同的ActionType，设置不同的默认值
	// ActionTypeHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FTheOneAbilityActionDataCustomLayout::OnActionTypeChanged));
	uint32 NumChildren;
	InStructPropertyHandle->GetNumChildren(NumChildren);
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = InStructPropertyHandle->GetChildHandle(ChildIndex);
		if (ChildHandle.IsValid())
		{
			if (ChildHandle->GetProperty()->GetName() == "CustomKVs")
			{
				// 绘制一个按钮
				StructBuilder.AddCustomRow(FText::FromString(""))
							 .NameContent()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("功能按键")))
						.Font(StructCustomizationUtils.GetRegularFont())
					]
					.ValueContent()
					.MinDesiredWidth(500.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
							.Text(FText::FromString(TEXT("默认值")))
							.OnClicked(FOnClicked::CreateSP(this, &FTheOneAbilityActionDataConfigCustomLayout::SetDefault))
						]
						// 绘制一个按钮，点击后，显示一组FString选择框
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							// Todo: 改成 SelectData For
							SNew(SButton)
							.Text(FText::FromString(TEXT("选择数据")))
							.OnClicked(FOnClicked::CreateSP(this, &FTheOneAbilityActionDataConfigCustomLayout::SelectData))
						]
						// 绘制一个下拉框，可以选择一些FName, 并且作为临时数据存储
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SComboBox<TSharedPtr<FName>>)
							.OptionsSource(&CachedNameArray)
							.OnSelectionChanged(this, &FTheOneAbilityActionDataConfigCustomLayout::OnKeySelected)
							.OnGenerateWidget_Lambda([](TSharedPtr<FName> InItem)
							{
								return SNew(STextBlock).Text(FText::FromName(*InItem));
							})
							.Content()
							[
								SNew(STextBlock)
								.Text_Lambda([this]() -> FText
								{
									return FText::FromName(CacheSelectedKeyHandle);
								})
							]
						]
					];
			}
			StructBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}
}

void FTheOneAbilityActionDataConfigCustomLayout::OnKeySelected(TSharedPtr<FName> Name, ESelectInfo::Type Arg)
{
	if (Name.IsValid())
	{
		CacheSelectedKeyHandle = *Name;
	}
}

void FTheOneAbilityActionDataConfigCustomLayout::AddKVRow(TSharedPtr<IPropertyHandleMap> MapHandle, FName Key)
{
	MapHandle->AddItem();
	uint32 LastIndex;
	MapHandle->GetNumElements(LastIndex);
	LastIndex -= 1;
	TSharedPtr<IPropertyHandle> NewElementHandle = MapHandle->GetElement(LastIndex);
	NewElementHandle->GetKeyHandle()->SetValue(Key);
}

FReply FTheOneAbilityActionDataConfigCustomLayout::SetDefault()
{
	UE_LOG(LogTemp, Warning, TEXT("SetDefault"));
	// ActionDataHandle对应的变量是FTheOneAbilityActionData结构体， 这个函数用于设置它的默认值
	if (ActionTypeHandle.IsValid() && ActionTypeHandle->IsValidHandle())
	{
		uint8 EnumUint;
		ActionTypeHandle->GetValue(EnumUint);
		ETheOneAbilityActionType CurrentType = (ETheOneAbilityActionType)EnumUint;

		//  保留代码， 设置Array类型数据
		// if (CustomKVsHandle.IsValid())
		// {
		// 	auto ArrayHandle = CustomKVsHandle->AsArray();
		// 	ArrayHandle->EmptyArray();
		// 	// Add a new element to the array
		// 	ArrayHandle->AddItem();
		//
		// 	// Get the handle of the newly added element
		// 	uint32 LastIndex;
		// 	ArrayHandle->GetNumElements(LastIndex);
		// 	LastIndex -= 1;
		//
		// 	TSharedPtr<IPropertyHandle> NewElementHandle = ArrayHandle->GetElement(LastIndex);
		// 	TSharedPtr<IPropertyHandle> KeyHandle = NewElementHandle->GetChildHandle("Key");
		// 	TSharedPtr<IPropertyHandle> ValueHandle = NewElementHandle->GetChildHandle("Value");
		// 	KeyHandle->SetValueFromFormattedString(TheOneGameplayTags::Debug_GE.GetTag().ToString());
		// 	ValueHandle->SetValueFromFormattedString("100");
		//
		// 	// auto MapHandle = CustomKVsHandle->AsMap();
		// 	// MapHandle->Empty();
		// 	// // Add a new element to the Map
		// 	// MapHandle->AddItem();
		// 	// // Get the handle of the newly added element
		// 	// uint32 LastIndex;
		// 	// MapHandle->GetNumElements(LastIndex);
		// 	// LastIndex -= 1;
		// 	// TSharedPtr<IPropertyHandle> NewElementHandle = MapHandle->GetElement(LastIndex);
		// 	// auto KeyHandle = NewElementHandle->GetKeyHandle();
		// 	// KeyHandle->SetValue(TheOneGameplayTags::Debug_GE.GetTag().ToString());
		// 	// auto ValueHandle = NewElementHandle->GetValueHandle();
		// }

		auto MapHandle = CustomKVsHandle->AsMap();
		MapHandle->Empty();
		
		if (CustomKVsHandle.IsValid() && CustomKVsHandle->IsValidHandle())
		{
			// TestHandle的类型为TMap<FName, FString>， 增加一个元素， 并设置默认值
			// MapHandle->AddItem();
			// MapHandle->GetNumElements(LastIndex);
			// LastIndex -= 1;
			// NewElementHandle = MapHandle->GetElement(LastIndex);
			// NewElementHandle->GetKeyHandle()->SetValue(FName("DefaultKey2"));
			// // 通过;分割，代表各个等级的数值
			// UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="ActionType == ETheOneAbilityActionType::Damage", ClampMin="0.0"))
			// FString DamagePercent;
			//
			// UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="ActionType == ETheOneAbilityActionType::FixedDamage", ClampMin="0.0"))
			// FString FixedDamage;
			//
			// UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="ActionType == ETheOneAbilityActionType::Damage||ActionType == ETheOneAbilityActionType::FixedDamage"))
			// FGameplayTag DamageTag;
			//
			// UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="ActionType == ETheOneAbilityActionType::Spawn"))
			// FVector SpawnOffset;
			//
			// UPROPERTY(EditAnywhere, meta=(EditConditionHides,EditCondition="ActionType == ETheOneAbilityActionType::Spawn"))
			// FDataTableRowHandle SpawnActorRowHandle;
		}


		switch (CurrentType)
		{
			case ETheOneAbilityActionType::Invalid:
				UE_LOG(LogTemp, Warning, TEXT("Invalid"));
				break;
			case ETheOneAbilityActionType::Damage:
				UE_LOG(LogTemp, Warning, TEXT("Damage"));
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::ExpressionKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::DamageTagKey);
				break;
			case ETheOneAbilityActionType::SpawnProjectile:
				UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile"));
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::ProjectileRowNameKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::SpawnSocketNameKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::SpawnOffsetKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::FlyTypeKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::FlySpeedKey);
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::HitActionsKey); // Todo: 使用统一的分隔符
				break;
			case ETheOneAbilityActionType::AddBuff:
				UE_LOG(LogTemp, Warning, TEXT("AddBuffToTarget"));
				AddKVRow(MapHandle, FTheOneAbilityActionDataConfig::BuffRowNameKey);
				break;
		}
	}
	return FReply::Handled();
}

void FTheOneAbilityActionDataConfigCustomLayout::OnTagSelected(FName InValue)
{
	if (CustomKVsHandle.IsValid() && CustomKVsHandle->IsValidHandle())
	{
		// 查询Key为DefaultKey2的元素
		auto MapHandle = CustomKVsHandle->AsMap();
		uint32 NumElements;
		MapHandle->GetNumElements(NumElements);
		for (uint32 Index = 0; Index < NumElements; ++Index)
		{
			TSharedPtr<IPropertyHandle> ElementHandle = MapHandle->GetElement(Index);
			FName Key;
			ElementHandle->GetKeyHandle()->GetValue(Key);
			if (Key == CacheSelectedKeyHandle)
			{
				ElementHandle->SetValue(InValue);
				break;
			}
		}
	}
}

FReply FTheOneAbilityActionDataConfigCustomLayout::SelectData()
{
	// 创建一个下拉框，用于选择Tag
	// 打印CacheSelectedKeyHandle
	FMenuBuilder MenuBuilder(true, nullptr);
	// 获取所有可用的标签
	TArray<FName> PossibleValues;
	if (CacheSelectedKeyHandle.IsEqual(FTheOneAbilityActionDataConfig::DamageTagKey) )
	{
		PossibleValues.Add(TheOneGameplayTags::SetByCaller_Damage_Melee.GetTag().GetTagName());
		PossibleValues.Add(TheOneGameplayTags::SetByCaller_Damage_Range.GetTag().GetTagName());
	}
	else if (CacheSelectedKeyHandle.IsEqual(FTheOneAbilityActionDataConfig::ProjectileRowNameKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileRowNameKey"));
		auto ProjectileTable = GetDefault<UTheOneDataTableSettings>()->ProjectileTable.LoadSynchronous();
		if (ProjectileTable)
		{
			auto RowNames = ProjectileTable->GetRowNames();
			for (const auto& InRowName : RowNames)
			{
				PossibleValues.Add(InRowName);
			}
		}
	}
	else if (CacheSelectedKeyHandle.IsEqual(FTheOneAbilityActionDataConfig::SpawnSocketNameKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnSocketNameKey"));
		PossibleValues.Add(TEXT("暂未定义"));
	}
	else if (CacheSelectedKeyHandle.IsEqual(FTheOneAbilityActionDataConfig::FlyTypeKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("FlyTypeKey"));
		PossibleValues.Add(UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType::Trace));
		PossibleValues.Add(UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType::Line));
		PossibleValues.Add(UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType::Lob));
		PossibleValues.Add(UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType::FixedLocation));
		PossibleValues.Add(UTheOneBlueprintFunctionLibrary::ProjectileFlyTypeToName(ETheOneProjectileFlyType::Instant));
		
	}
	else if (CacheSelectedKeyHandle.IsEqual(FTheOneAbilityActionDataConfig::BuffRowNameKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffRowNameKey"));
		auto BuffTable = GetDefault<UTheOneDataTableSettings>()->BuffTable.LoadSynchronous();
		if (BuffTable)
		{
			auto RowNames = BuffTable->GetRowNames();
			for (const auto& InRowName : RowNames)
			{
				PossibleValues.Add(InRowName);
			}
		}
	}
	
	

	for (const auto& InValue : PossibleValues)
	{
		MenuBuilder.AddMenuEntry(
			FText::FromName(InValue),
			FText::FromString("Select " + InValue.ToString()),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateSP(this, &FTheOneAbilityActionDataConfigCustomLayout::OnTagSelected, InValue))
		);
	}

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