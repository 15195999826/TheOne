// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModeComponents/TheOneLuaBattleComponent.h"

#include "Subsystems/TheOneLuaEnvSystem.h"

// Sets default values for this component's properties
UTheOneLuaBattleComponent::UTheOneLuaBattleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTheOneLuaBattleComponent::BeginPlay()
{
	// ...
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginPlay From C++"));
	Super::BeginPlay();

	// auto LuaEnvSystem = GetWorld()->GetSubsystem<UTheOneLuaEnvSystem>();
	// LuaEnvSystem->TestCallLuaByGlobalTable();
	// LuaEnvSystem->TestCallLuaByFLuaTable();
}


// Called every frame
void UTheOneLuaBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	// OnTest.Broadcast("Hello MultiCastDelegate");
}

