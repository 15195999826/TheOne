// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TheOneTeamSystem.h"

#include "TheOneLogChannels.h"
#include "AI/TheOneAIController.h"
#include "Game/TheOneGameModeBase.h"
#include "Item/TheOneItemSystem.h"
#include "Kismet/GameplayStatics.h"

void UTheOneTeamSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	PlayerTeamID = CreateTeam();
	auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
	ItemSystem->OnPostItemUpdated.AddUObject(this, &UTheOneTeamSystem::OnPostItemUpdated);
}

int32 UTheOneTeamSystem::CreateTeam()
{
	NextTeamID++;
	Teams.Add(NextTeamID, TArray<uint32>());
	return NextTeamID;
}

uint32 UTheOneTeamSystem::AddCharacterToTeam(int32 InTeamID, const FName& InCharacterRowName, bool bCreateItemHook)
{
	// 查询到该队伍第一个空位
	const auto* Team = Teams.Find(InTeamID);
	if (Team == nullptr)
	{
		UE_LOG(LogTheOne, Error, TEXT("AddCharacterToTeam Failed, TeamID %d Not Exist"), InTeamID);
		return INDEX_NONE;
	}

	// 队伍共24个位置
	int32 FirstEmptyPosition = INDEX_NONE;
	for (int i = 0; i < 24; i++)
	{
		bool HasCharacterAtPosition = false;
		for (const auto& Flag : *Team)
		{
			if (CharacterUniques[Flag].TeamPosition == i)
			{
				HasCharacterAtPosition = true;
				break;
			}
		}

		if (HasCharacterAtPosition == false)
		{
			FirstEmptyPosition = i;
			break;
		}
	}

	return  AddCharacterToTeam(InTeamID, InCharacterRowName, FirstEmptyPosition, bCreateItemHook);
}

uint32 UTheOneTeamSystem::AddCharacterToTeam(int32 InTeamID, const FName& InCharacterRowName, int InTeamPosition,
	bool bCreateItemHook)
{
	check(InTeamPosition > -1 && InTeamPosition < 24);
	if (Teams.Contains(InTeamID) == false)
	{
		UE_LOG(LogTheOne, Error, TEXT("AddCharacterToTeam Failed, TeamID %d Not Exist"), InTeamID);
		return INDEX_NONE;
	}

	auto& Team = Teams[InTeamID];

	// 创建一个CharacterActor, 在屏幕可见范围外
	auto GM = Cast<ATheOneGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnStartLocation + GM->SpawnedAIMap.Num() * FVector(200, 0, 0));
	FTheOneAIPawnSpawnInfo SpawnInfo;
	SpawnInfo.Camp = InTeamID == PlayerTeamID ? ETheOneCamp::Player : ETheOneCamp::Enemy;
	SpawnInfo.CharacterTemplateRowName = InCharacterRowName;
	
	FTheOneCharacterUnique CharacterUnique;
	// 生成一个随机的名字， FText类型
	CharacterUnique.Name = FText::FromString(FString::Printf(TEXT("随从%d"), GM->SpawnedAIMap.Num()));
	const auto& Ctrl = GM->SpawnOneConfigAIAtTransform(SpawnTransform, SpawnInfo);
	auto Flag = Ctrl->GetUniqueID();
	CharacterUnique.Flag = Flag;
	CharacterUnique.TeamPosition = InTeamPosition;
	Team.Add(CharacterUnique.Flag);
	CharacterUniques.Add(CharacterUnique.Flag, CharacterUnique);
	// 创建一个衔接的道具实例
	if (bCreateItemHook)
	{
		auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
		int32 ToSlotID = INDEX_NONE;
		if ( InTeamID == PlayerTeamID)
		{
			ToSlotID = ItemSystem->GetPlayerTeamSlotIDs()[InTeamPosition];
		}
		
		ItemSystem->CreateItemInstance(InCharacterRowName, ETheOneItemType::Minion, ToSlotID, [this, Flag](int32 ItemID)
		{
			UE_LOG(LogTheOne, Log, TEXT("CreateItemInstance ID: %d, Hook Flag: %d"), ItemID, Flag);
			Character2ItemHookMap.Add(Flag, ItemID);
			Item2CharacterHookMap.Add(ItemID, Flag);
		});
		
	}

	return CharacterUnique.Flag;
}

const FTheOneCharacterUnique& UTheOneTeamSystem::GetCharacterUniqueByHookedItemID(int32 InItemID) const
{
	if (Item2CharacterHookMap.Contains(InItemID) == false)
	{
		UE_LOG(LogTheOne, Error, TEXT("GetCharacterUniqueByHookedItemID Failed, ItemID %d Not Exist"), InItemID);
		static FTheOneCharacterUnique Empty;
		return Empty;
	}

	auto Flag = Item2CharacterHookMap[InItemID];
	return GetCharacterUnique(Flag);
}

const FTheOneCharacterUnique& UTheOneTeamSystem::GetCharacterUnique(uint32 InFlag) const
{
	if (CharacterUniques.Contains(InFlag) == false)
	{
		UE_LOG(LogTheOne, Error, TEXT("GetCharacterUnique Failed, Flag %d Not Exist"), InFlag);
		static FTheOneCharacterUnique Empty;
		return Empty;
	}

	return CharacterUniques[InFlag];
}

TArray<uint32> UTheOneTeamSystem::GetTeam(int32 InTeamID) const
{
	if (Teams.Contains(InTeamID) == false)
	{
		UE_LOG(LogTheOne, Error, TEXT("GetTeamCharacters Failed, TeamID %d Not Exist"), InTeamID);
		return TArray<uint32>();
	}

	return Teams[InTeamID];
}

// 初次创建不会走到这个函数， 初次创建时，角色会被放置在正确的位置上
void UTheOneTeamSystem::OnPostItemUpdated(int OldSlotID, const FTheOneItemInstance& TheOneItemInstance)
{
	if (TheOneItemInstance.ItemType != ETheOneItemType::Minion)
	{
		return;
	}

	auto HookedCharacterFlag = Item2CharacterHookMap[TheOneItemInstance.ItemID];
	
	// 如果是玩家队伍
	if (Teams[PlayerTeamID].Contains(HookedCharacterFlag))
	{
		auto ItemSystem = GetWorld()->GetSubsystem<UTheOneItemSystem>();
		const auto& PlayerTeamSlots = ItemSystem->GetPlayerTeamSlotIDs();
		auto CurrentSlotIndex = PlayerTeamSlots.IndexOfByKey(TheOneItemInstance.LogicSlotID);
		CharacterUniques[HookedCharacterFlag].TeamPosition = CurrentSlotIndex;
	}
}
