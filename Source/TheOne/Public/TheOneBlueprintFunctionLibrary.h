// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "TheOneValidEnum.h"
#include "Actor/TheOneProjectileActor.h"
#include "Game/TheOnePlayerControllerBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/TheOneAdditionMesh.h"
#include "TheOneBlueprintFunctionLibrary.generated.h"

enum class ETheOneImportantUI : uint8;
struct FGameplayAbilitySpecHandle;
class UTheOneGeneralGA;
enum class ETheOneUIOverlayType : uint8;
class ATheOneCharacterBase;
enum class ETheOneHiBoxType;
class UGameplayEffect;
class AHexGrid;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static FActiveGameplayEffectHandle EmptyActiveGameplayEffectHandle = FActiveGameplayEffectHandle();

	static void AssertIsTrue(bool b, const FString& Message);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsCharacterConfigValid(FName InRowName);

	static FGameplayEffectSpecHandle SimpleMakeGESpecHandle(AActor* Source, TSubclassOf<UGameplayEffect> GEClass, float Level = 1.0f);
	
	static FActiveGameplayEffectHandle SimpleApplyGEToSelf(AActor* Source, TSubclassOf<UGameplayEffect> GE,
														   float Level = 1.0f);
	static ATheOneCharacterBase* FindCharacter(const UObject* WorldContextObject, uint32 InFlag);
	
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static AHexGrid* GetHexGrid(const UObject* WorldContextObject);

	static bool TheOneGiveAbility(UAbilitySystemComponent* InASC, const FName& InAbilityRowName, FGameplayAbilitySpecHandle& OutGAHandle, UTheOneGeneralGA*& OutGAInstance);
	
	static void AttachAdditionMeshToCharacter(ATheOneCharacterBase* InCharacter, const FTheOneAdditionMesh& InAdditionMesh, FName InTag = NAME_None);

	static FGameplayTag CastToDamageTypeTag(ETheOneDamageType InDamageType);
	
	static FName ProjectileFlyTypeToName(ETheOneProjectileFlyType InFlyType);

	static ETheOneProjectileFlyType NameToProjectileFlyType(FName InName);

	static bool TryFindSocketLocation(AActor* InActor, const FName& InSocketName, FVector& OutLocation);
	
	static FTheOneAbilityActionPoint CastConfigToAbilityActionPoint(const FTheOneActionPointConfig& InActionPoint);

	static FTheOneAbilityActionContainer CastConfigToAbilityActionContainer(const FTheOneAbilityActionContainerConfig& InActionContainer);

private:
	static FTheOneAbilityAction CastConfigToAbilityAction(const FTheOneAbilityActionConfig& InAction);
	
	static FTheOneAbilityActionData CastConfigToAbilityActionData(const FTheOneAbilityActionDataConfig& InActionData);

	// For Item
public:
	inline static TMap<ETheOneCharacterBagSlotType, FName> BagSlotTypeToSocketName = {
		{ETheOneCharacterBagSlotType::MainHand, "MainHandSocket"},
		{ETheOneCharacterBagSlotType::OffHand, "OffHandSocket"},
		{ETheOneCharacterBagSlotType::Head, "HeadSocket"},
		{ETheOneCharacterBagSlotType::Cloth, "ClothSocket"},
		{ETheOneCharacterBagSlotType::LeftJewelry, "LeftJewelrySocket"},
		{ETheOneCharacterBagSlotType::RightJewelry, "RightJewelrySocket"},
	};
	
	static void CharacterBagSlotIDToType(int32 InSlotID, ETheOneCharacterBagSlotType& OutType, int32& OutIndex);
	
	static void Equip(ATheOneCharacterBase* InCharacter, int32 InEquipmentID, ETheOneCharacterBagSlotType InSlotType);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool CanItemDropOnCharacterBagSlot(int32 InSlotID, const FName& InItemRowName);
private:
	static void UnEquip(ATheOneCharacterBase* InCharacter, ETheOneCharacterBagSlotType InSlotType);
	
	// For HitBox	
public:
	static TArray<AActor*> GetHitBoxHitResult(
		const UObject* WorldContextObject,
		const FVector& SourceLocation,
		const FVector& SourceForward,
		const FRotator& SourceRotation,
		ETheOneHiBoxType HitBoxType,
		bool HitMulti,
		const FVector& BoxSize,
		float SectorAngle,
		float Radius,
		float AboveGround,
		float StartDistance,
		float EndDistance,
		bool IgnoreSelf,
		bool DrawDebug);

private:
	static void BoxHits(
		const UObject* WorldContextObject,
		const FRotator& Orientation,
		const FVector& StartLocation,
		const FVector& EndLocation,
		const FVector& BoxSize,
		TArray<AActor*> ActorsToIgnore,
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
		TArray<AActor*>& InRangeHitActors,
		bool IgnoreSelf,
		bool DrawDebug);

	static void SectorHits(
		const UObject* WorldContextObject,
		const FVector& Forward,
		const FVector& StartLocation,
		float SectorAngle,
		float Radius,
		TArray<AActor*> ActorsToIgnore,
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
		TArray<AActor*>& InRangeHitActors,
		bool IgnoreSelf,
		bool DrawDebug);

	static void CircleHits(
		const UObject* WorldContextObject,
		const FVector& StartLocation,
		float Radius,
		TArray<AActor*> ActorsToIgnore,
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes,
		TArray<AActor*>& InRangeHitActors,
		bool IgnoreSelf,
		bool DrawDebug);

	// For UI
public:
	UFUNCTION(BlueprintCallable, Category="TheOne|UI")
	static void EnableUINav();
	UFUNCTION(BlueprintCallable, Category="TheOne|UI")
	static void DisableUINav();
	
	UFUNCTION(BlueprintCallable, Category="TheOne|UI", meta=(WorldContext="WorldContextObject"))
	static void PushUI(const UObject* WorldContextObject,
		ETheOneUIOverlayType OverlayType,
		UUserWidget* Widget,
		TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill,
		TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill);
	UFUNCTION(BlueprintCallable, Category="TheOne|UI", meta=(WorldContext="WorldContextObject"))
	static void ShowImportantUI(const UObject* WorldContextObject, ETheOneImportantUI InUI);
	UFUNCTION(BlueprintCallable, Category="TheOne|UI", meta=(WorldContext="WorldContextObject"))
	static void CloseImportantUI(const UObject* WorldContextObject, ETheOneImportantUI InUI);
};
