// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TheOneItemSystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TheOneCharacterUnique.h"
#include "TheOneTeamSystem.generated.h"

/**
 * 
 */
UCLASS()
class THEONE_API UTheOneTeamSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	inline static FVector SpawnStartLocation = FVector(-14600.0, 14600, 100);

protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
protected:
	int32 NextTeamID = 0;
	

	int32 PlayerTeamID = 0;

	// <TeamID, 队伍成员>
	TMap<int32, TArray<uint32>> Teams;
	TMap<uint32, FTheOneCharacterUnique> CharacterUniques;

	// 有些角色需要变成道具的形式， 放到背包或者商店中， 比如玩家角色和招募商店的角色， 这里记录角色和道具ID的映射关系
	TMap<uint32, int32> Character2ItemHookMap;
	TMap<int32, uint32> Item2CharacterHookMap;
public:
	int32 GetPlayerTeamID() const
	{
		return PlayerTeamID;
	}

	int32 GetHookedItemID(uint32 InFlag) const
	{
		if (Character2ItemHookMap.Contains(InFlag))
		{
			return Character2ItemHookMap[InFlag];
		}

		return INDEX_NONE;
	}
	
	int32 CreateTeam();

	uint32 AddCharacterToTeam(int32 InTeamID, const FName& InCharacterRowName, bool bCreateItemHook = false);
	uint32 AddCharacterToTeam(int32 InTeamID, const FName& InCharacterRowName, int InTeamPosition, bool bCreateItemHook = false);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FTheOneCharacterUnique& GetCharacterUniqueByHookedItemID(int32 InItemID) const;
	
	const FTheOneCharacterUnique& GetCharacterUnique(uint32 InFlag) const;

	TArray<uint32> GetTeam(int32 InTeamID) const; 
	
private:
	void OnPostItemUpdated(int OldSlotID, const FTheOneItemInstance& TheOneItemInstance);
};
