// Fill out your copyright notice in the Description page of Project Settings.


#include "TheOneCustomEditorSubsystem.h"

#include "EditorStyleSet.h"
#include "IDetailTreeNode.h"
#include "LevelEditor.h"

void UTheOneCustomEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// RegisterDetailPanelCustomization();
}

void UTheOneCustomEditorSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// UnregisterDetailPanelCustomization();
}

void UTheOneCustomEditorSubsystem::OnExtendLevelEditorMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("TheOneEditor", NSLOCTEXT("TheOneEditor", "TheOneEditor","TheOne"));
	MenuBuilder.AddMenuEntry(
		NSLOCTEXT("TheOneEditor", "TheOneEditor", "TheOne Editor"),
		NSLOCTEXT("TheOneEditor", "TheOneEditor", "Open TheOne Editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([]()
			{
				UE_LOG(LogTemp, Error, TEXT("Open TheOne Editor"));
			})
		));
}

void UTheOneCustomEditorSubsystem::RegisterDetailPanelCustomization()
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	OnGetGlobalRowExtensionHandle = PropertyEditor.GetGlobalRowExtensionDelegate().AddUObject(this, &UTheOneCustomEditorSubsystem::OnGetGlobalRowExtension);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	
	MenuExtender = MakeShareable(new FExtender);
	MenuExtender->AddMenuExtension("EditText",
		EExtensionHook::Before,
		TSharedPtr<FUICommandList>(),
		FMenuExtensionDelegate::CreateUObject(this, &UTheOneCustomEditorSubsystem::OnExtendLevelEditorMenu));
	
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void UTheOneCustomEditorSubsystem::UnregisterDetailPanelCustomization()
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditor.GetGlobalRowExtensionDelegate().Remove(OnGetGlobalRowExtensionHandle);
	OnGetGlobalRowExtensionHandle.Reset();
}

// 可以通过该方式，扩展Property最右侧菜单功能
void UTheOneCustomEditorSubsystem::OnGetGlobalRowExtension(
	const FOnGenerateGlobalRowExtensionArgs& InArgs,
	TArray<FPropertyRowExtensionButton>& OutExtensions)
{
	TSharedPtr<IPropertyHandle> PropertyHandle = InArgs.PropertyHandle;
	auto OwnerTreeNode = InArgs.OwnerTreeNode;
	if (!PropertyHandle.IsValid())
	{
		
		return;
	}
	// 打印Property的名字
	UE_LOG(LogTemp, Error, TEXT("Property Name: %s"), *PropertyHandle->GetPropertyDisplayName().ToString());
	
	// 检查是否FName类型
	// auto CurProperty = PropertyHandle->GetProperty();
	// if (CurProperty == nullptr || CastField<FNameProperty>(CurProperty) == nullptr)
	// {
	// 	return;
	// }
	//
	// FString JumpTableName;
	// //查询元数据中的内容
	// if (PropertyHandle->HasMetaData(TEXT("JumpTableName")))
	// {
	// 	JumpTableName = PropertyHandle->GetMetaData(TEXT("JumpTableName"));
	// }
	// if (PropertyHandle->AsArray() != nullptr) return;
	// if (PropertyHandle->IsValidHandle() == false) return;
	// if (JumpTableName.IsEmpty()) return;
	

	// 添加Expose按钮
	FPropertyRowExtensionButton& AnimatePropertyButton = OutExtensions.AddDefaulted_GetRef();
	AnimatePropertyButton.Label = FText::FromString("Jump to table");
	AnimatePropertyButton.ToolTip = FText::FromString("Jump to target table");
	AnimatePropertyButton.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "PropertyWindow.Button_Browse");
}

TSharedPtr<SWidget> UTheOneCustomEditorSubsystem::GetExposeButton(const FOnGenerateGlobalRowExtensionArgs& InArgs,
	FString InTableName)
{
	return SNew(SBox)
		.MinDesiredWidth(1)
		.ToolTipText(FText::FromString("Jump to target table"))
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.IsFocusable(false)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.OnClicked(FOnClicked::CreateLambda([this, InArgs, InTableName] ()
				{
					FString TableName = InTableName;
					TSharedPtr<IPropertyHandle> CurHandle = InArgs.PropertyHandle;
					int32 OutID = 0;
					if (CurHandle->GetValue(OutID) == FPropertyAccess::Result::Success)
					{
						//跳转到Table的函数
						JumpToTable(TableName, OutID);
					}
					return FReply::Handled();
				})
			)
			[
				SNew(SImage).Image(FAppStyle::GetBrush("PropertyWindow.Button_Browse"))
			]
		];
}

void UTheOneCustomEditorSubsystem::JumpToTable(const FString& TableName, int32 ID)
{
	UE_LOG(LogTemp, Error, TEXT("Jump to table %s, ID: %d"), *TableName, ID);
}
