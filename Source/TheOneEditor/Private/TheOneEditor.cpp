#include "TheOneEditor.h"


#include "AbilitySystemGlobals.h"
#include "DataValidationModule.h"
#include "Development/TheOneDeveloperSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "TheOneGameEditorStyle.h"
// #include "GameModes/LyraExperienceManager.h"
#include "GameplayAbilitiesEditorModule.h"
#include "GameplayCueInterface.h"
#include "GameplayCueNotify_Burst.h"
#include "GameplayCueNotify_BurstLatent.h"
#include "GameplayCueNotify_Looping.h"
// #include "Private/AssetTypeActions_LyraContextEffectsLibrary.h"
#include "DetailRowMenuContext.h"
#include "TheOneAbilityActionDataConfigCustomLayout.h"
#include "TheOneAbilityRowCustomLayout.h"
#include "TheOneCharacterAbilityCustomLayout.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"

// #include "Validation/EditorValidator.h"

#define LOCTEXT_NAMESPACE "FTheOneEditorModule"

// This function tells the GameplayCue editor what classes to expose when creating new notifies.
static void GetGameplayCueDefaultClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();
	Classes.Add(UGameplayCueNotify_Burst::StaticClass());
	Classes.Add(AGameplayCueNotify_BurstLatent::StaticClass());
	Classes.Add(AGameplayCueNotify_Looping::StaticClass());
}

// This function tells the GameplayCue editor what classes to search for GameplayCue events.
static void GetGameplayCueInterfaceClasses(TArray<UClass*>& Classes)
{
	Classes.Empty();

	for (UClass* Class : TObjectRange<UClass>())
	{
		if (Class->IsChildOf<AActor>() && Class->ImplementsInterface(UGameplayCueInterface::StaticClass()))
		{
			Classes.Add(Class);
		}
	}
}

// This function tells the GameplayCue editor where to create the GameplayCue notifies based on their tag.
static FString GetGameplayCuePath(FString GameplayCueTag)
{
	FString Path = FString(TEXT("/Game"));

	//@TODO: Try plugins (e.g., GameplayCue.ShooterGame.Foo should be in ShooterCore or something)

	// Default path to the first entry in the UAbilitySystemGlobals::GameplayCueNotifyPaths.
	if (IGameplayAbilitiesModule::IsAvailable())
	{
		IGameplayAbilitiesModule& GameplayAbilitiesModule = IGameplayAbilitiesModule::Get();

		if (GameplayAbilitiesModule.IsAbilitySystemGlobalsAvailable())
		{
			UAbilitySystemGlobals* AbilitySystemGlobals = GameplayAbilitiesModule.GetAbilitySystemGlobals();
			check(AbilitySystemGlobals);

			TArray<FString> GetGameplayCueNotifyPaths = AbilitySystemGlobals->GetGameplayCueNotifyPaths();

			if (GetGameplayCueNotifyPaths.Num() > 0)
			{
				Path = GetGameplayCueNotifyPaths[0];
			}
		}
	}

	GameplayCueTag.RemoveFromStart(TEXT("GameplayCue."));

	FString NewDefaultPathName = FString::Printf(TEXT("%s/GCN_%s"), *Path, *GameplayCueTag);

	return NewDefaultPathName;
}


static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static void OpenCommonMap_Clicked(const FString MapPath)
{
	if (ensure(MapPath.Len()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
	}
}

static bool CanShowCommonMaps()
{
	return HasNoPlayWorld() && !GetDefault<UTheOneDeveloperSettings>()->CommonEditorMaps.IsEmpty();
}

static void TestAction(UToolMenu* InMenu)
{
	UE_LOG(LogTemp, Warning, TEXT("TestAction"));

	const FToolMenuContext Context = InMenu->Context;
	if (Context.IsEditing())
	{
		UE_LOG(LogTemp, Warning, TEXT("Context.IsValid()"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Context.IsNotValid()"));
	}
}

static TSharedRef<SWidget> GetCommonMapsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	for (const FSoftObjectPath& Path : GetDefault<UTheOneDeveloperSettings>()->CommonEditorMaps)
	{
		if (!Path.IsValid())
		{
			continue;
		}
		
		const FText DisplayName = FText::FromString(Path.GetAssetName());
		MenuBuilder.AddMenuEntry(
			DisplayName,
			LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Path.ToString()),
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
			)
		);
	}

	return MenuBuilder.MakeWidget();
}


static void RegisterGameEditorMenus()
{
	// UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	// FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	// Uncomment this to add a custom toolbar that is displayed during PIE
	// Useful for making easy access to changing game state artificially, adding cheats, etc
	// FToolMenuEntry BlueprintEntry = FToolMenuEntry::InitComboButton(
	// 	"OpenGameMenu",
	// 	FUIAction(
	// 		FExecuteAction(),
	// 		FCanExecuteAction::CreateStatic(&HasPlayWorld),
	// 		FIsActionChecked(),
	// 		FIsActionButtonVisible::CreateStatic(&HasPlayWorld)),
	// 	FOnGetContent::CreateStatic(&YourCustomMenu),
	// 	LOCTEXT("GameOptions_Label", "Game Options"),
	// 	LOCTEXT("GameOptions_ToolTip", "Game Options"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.OpenLevelBlueprint")
	// );
	// BlueprintEntry.StyleNameOverride = "CalloutToolbar";
	// Section.AddEntry(BlueprintEntry);

	// 有插件实现了更方便的使用
	// FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
	// 	"CommonMapOptions",
	// 	FUIAction(
	// 		FExecuteAction(),
	// 		FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
	// 		FIsActionChecked(),
	// 		FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)),
	// 	FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
	// 	LOCTEXT("CommonMaps_Label", "Common Maps"),
	// 	LOCTEXT("CommonMaps_ToolTip", "Some commonly desired maps while using the editor"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
	// );
	// CommonMapEntry.StyleNameOverride = "CalloutToolbar";
	// Section.AddEntry(CommonMapEntry);
	
	// UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(
	// 		"PropertyEditor.RowContextMenu");
	// ToolbarMenu->AddSection(
	// 			   TEXT("TheOne"),
	// 			   NSLOCTEXT("TheOneEditor", "TheOneHeading", "TheOne"),
	// 			   FToolMenuInsert(TEXT("Edit"), EToolMenuInsertType::After));
	//
	// FToolMenuSection& ExpansionSection = ToolbarMenu->FindOrAddSection(TEXT("TheOne"));
	//
	// const FUIAction CollapseAllAction( FExecuteAction::CreateStatic(&TestAction, ToolbarMenu) );
	// ExpansionSection.AddMenuEntry(
	// 	TEXT("Test"),
	// 	NSLOCTEXT("PropertyView", "CollapseAll", "Collapse All"),
	// 	NSLOCTEXT("PropertyView", "CollapseAll_ToolTip", "Collapses this item and all children"),
	// 	FSlateIcon(),
	// 	CollapseAllAction);
}

class FTheOneEditorModule : public FDefaultGameModuleImpl
{
	typedef FTheOneEditorModule ThisClass;

	virtual void StartupModule() override
	{
		FTheOneGameEditorStyle::Initialize();
		
		if (!IsRunningGame())
		{
			FModuleManager::Get().OnModulesChanged().AddRaw(this, &FTheOneEditorModule::ModulesChangedCallback);
		
			BindGameplayAbilitiesEditorDelegates();
		
			if (FSlateApplication::IsInitialized())
			{
				ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
			}
		
			FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
			FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
		}

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout("TheOneCharacterAbilityTemplate", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTheOneCharacterAbilityCustomLayout::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("TheOneAbilityRow", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTheOneAbilityRowCustomLayout::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout("TheOneAbilityActionDataConfig", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTheOneAbilityActionDataConfigCustomLayout::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();

		FToolMenuOwnerScoped OwnerScoped(this);
		
	}

	void OnBeginPIE(bool bIsSimulating)
	{
	
	}

	void OnEndPIE(bool bIsSimulating)
	{
		
	}

	virtual void ShutdownModule() override
	{
		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);

		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			PropertyModule.UnregisterCustomPropertyTypeLayout("TheOneCharacterAbilityTemplate");
			PropertyModule.UnregisterCustomPropertyTypeLayout("TheOneAbilityRow");
			PropertyModule.UnregisterCustomPropertyTypeLayout("TheOneAbilityActionDataConfig");
		}
		
		// Undo UToolMenus
		if (UObjectInitialized() && ToolMenusHandle.IsValid())
		{
			UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
		}
		
		UnbindGameplayAbilitiesEditorDelegates();
		FModuleManager::Get().OnModulesChanged().RemoveAll(this);
		FTheOneGameEditorStyle::Shutdown();
	}

protected:

	static void BindGameplayAbilitiesEditorDelegates()
	{
		IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
		
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().BindStatic(&GetGameplayCueDefaultClasses);
		GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().BindStatic(&GetGameplayCueInterfaceClasses);
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().BindStatic(&GetGameplayCuePath);
	}

	static void UnbindGameplayAbilitiesEditorDelegates()
	{
		if (IGameplayAbilitiesEditorModule::IsAvailable())
		{
			IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
			GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().Unbind();
			GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().Unbind();
			GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().Unbind();
		}
	}

	void ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
	{
		if ((ReasonForChange == EModuleChangeReason::ModuleLoaded) && (ModuleThatChanged.ToString() == TEXT("GameplayAbilitiesEditor")))
		{
			BindGameplayAbilitiesEditorDelegates();
		}
	}

private:
	FDelegateHandle ToolMenusHandle;
	
private:
	void OnExpandAllClicked()
	{
		UE_LOG(LogTemp, Warning, TEXT("OnExpandAllClicked"));
	}
};
#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTheOneEditorModule, TheOneEditor)