#pragma once

class FTheOneCharacterAbilityCustomLayout : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() 
	{
		return MakeShareable( new FTheOneCharacterAbilityCustomLayout );
	}

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	TSharedPtr<IPropertyHandle> AbilityRowSelectorHandle;
	TSharedPtr<IPropertyHandle> AbilitiesHandle;
	void OnGetRowStrings(TArray<TSharedPtr<FString>>& OutStrings, TArray<TSharedPtr<SToolTip>>& OutToolTips, TArray<bool>& OutRestrictedItems);
	FString OnGetAbilitySelectorValueString() const;

	FReply OnAddNewAbilityToArray();
	

	void OnMenuItemSelected(FName Name);
	FReply OnSelectButtonClicked();
	
	static FString InternalOnGetRowValueString(TSharedPtr<IPropertyHandle> InHandle);
};
