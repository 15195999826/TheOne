// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameStateComponets/TheOneSpawnManagerComponent.h"

#include "EngineUtils.h"
#include "TheOneLogChannels.h"
#include "AI/TheOneAIController.h"
#include "GameFramework/PlayerStart.h"

// Sets default values for this component's properties
UTheOneSpawnManagerComponent::UTheOneSpawnManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTheOneSpawnManagerComponent::InitializeComponent()
{
	UE_LOG(LogTheOne, Log, TEXT("UTheOneSpawnManagerComponent::InitializeComponent"));
	Super::InitializeComponent();
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (APlayerStart* PlayerStart = *It)
		{
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

bool UTheOneSpawnManagerComponent::TryFindPlayerStart(const FName& Tag, AActor*& OutStart)
{
	for (auto PlayerStart : CachedPlayerStarts)
	{
		if (!PlayerStart.IsValid())
		{
			continue;
		}
		
		if (PlayerStart->PlayerStartTag == Tag)
		{
			OutStart = PlayerStart.Get();
			return true;
		}
	}

	return false;
}


// Called when the game starts
void UTheOneSpawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(LogTheOne, Log, TEXT("UTheOneSpawnManagerComponent::BeginPlay"));
}

void UTheOneSpawnManagerComponent::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld())
	{
		for (AActor* Actor : InLevel->Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				ensure(!CachedPlayerStarts.Contains(PlayerStart));
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	} else
	{
		UE_LOG(LogTheOne, Warning, TEXT("UTheOneSpawnManagerComponent::OnLevelAdded: InWorld != GetWorld(), InWorld: %s, GetWorld(): %s"), *InWorld->GetName(), *GetWorld()->GetName());
	}
}

void UTheOneSpawnManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (APlayerStart* PlayerStart = Cast<APlayerStart>(SpawnedActor))
	{
		CachedPlayerStarts.Add(PlayerStart);
	}
}

AActor* UTheOneSpawnManagerComponent::ChoosePlayerStart(AController* Player)
{
	// 查询CachedPlayerStarts
	if (auto AIController = Cast<ATheOneAIController>(Player))
	{
		for (auto PlayerStart : CachedPlayerStarts)
		{
			if (!PlayerStart.IsValid())
			{
				continue;
			}
			if (PlayerStart->PlayerStartTag == AIController->GetPawnSpawnInfo().StartTag)
			{
				return PlayerStart.Get();
			}
		}

		UE_LOG(LogTheOne, Error, TEXT("PlayerStart is invalid, StartTag: %s"), *AIController->GetPawnSpawnInfo().StartTag.ToString());
	}

	// 玩家Controller, 随便返回一个位置即可，按本游戏逻辑， 玩家Pawn将直接放置在场景中
	if (Cast<APlayerController>(Player))
	{
		if (CachedPlayerStarts.Num() > 0)
		{
			return CachedPlayerStarts[0].Get();
		}
	}
	
	return nullptr;
}
