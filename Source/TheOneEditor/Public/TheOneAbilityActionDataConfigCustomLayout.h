#pragma once
#include "AbilitySystem/TheOneAbilityAction.h"

class FTheOneAbilityActionDataConfigCustomLayout : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() 
	{
		return MakeShareable( new FTheOneAbilityActionDataConfigCustomLayout );
	}

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
private:
	inline static TArray<TSharedPtr<FName>> CachedNameArray {
		MakeShared<FName>(FTheOneAbilityActionDataConfig::DamageTagKey),
		MakeShared<FName>(FTheOneAbilityActionDataConfig::ProjectileRowNameKey),
		MakeShared<FName>(FTheOneAbilityActionDataConfig::SpawnSocketNameKey),
		MakeShared<FName>(FTheOneAbilityActionDataConfig::FlyTypeKey),
		MakeShared<FName>(FTheOneAbilityActionDataConfig::BuffRowNameKey)
	};
	
	FReply SetDefault();
	void OnTagSelected(FName InValue);
	FReply SelectData();
	
	TSharedPtr<IPropertyHandle> ActionTypeHandle;
	TSharedPtr<IPropertyHandle> CustomKVsHandle;

	
	void OnKeySelected(TSharedPtr<FName> Name, ESelectInfo::Type Arg);
	
	FName CacheSelectedKeyHandle; // 声明临时变量
	
	static void AddKVRow(TSharedPtr<IPropertyHandleMap> MapHandle, FName Key);
};
