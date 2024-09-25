// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AutoChess/AutoChessAIBBContext.h"

#include "TheOneBlueprintFunctionLibrary.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AI/AutoChess/TheOneAutoChessAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HexGrid/HexGrid.h"
#include "HexGrid/HGTypes.h"

bool UAutoChessAIBBContext::CanReleaseAbility(bool UseTargetBookedCoord)
{
	auto Self = Cast<ATheOneAutoChessAIController>(GetOuter());
	check(Self);

	if (TargetAI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s]CanReleaseAbility: TargetAI is nullptr"), *Self->GetName());
		return false;
	}
	
	// auto SelfCharacter = Cast<ATheOneCharacterBase>(Self->GetPawn());
	// check(SelfCharacter != nullptr);
	//
	// auto ToReleaseAbility = SelfCharacter->GetAbilityBySpecHandle(SelectedAbilitySpecHandle);
	// if (ToReleaseAbility == nullptr)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("[%s]CanReleaseAbility: ToReleaseAbility is nullptr"), *Self->GetName());
	// 	return false;
	// }
	//
	// switch (ToReleaseAbility->GetReleaseRule()) {
	// 	case ETheOneAbilityReleaseRule::Always:
	// 		return true;
	// 	case ETheOneAbilityReleaseRule::UseOwnerReleaseRange:
	// 		{
	// 			auto HexGrid = UTheOneBlueprintFunctionLibrary::GetHexGrid(this);
	// 			check(HexGrid);
	// 			auto OwnerCoord = Self->GetCurrentHexCoord();
	// 			FHCubeCoord TargetCoord;
	// 			if (UseTargetBookedCoord)
	// 			{
	// 				TargetCoord = TargetAI->GetBookedHexCoord();
	// 			}
	// 			else
	// 			{
	// 				TargetCoord = TargetAI->GetCurrentHexCoord();
	// 			}
	// 			auto Distance = HexGrid->GetDistance(OwnerCoord, TargetCoord);
	// 			if (Distance <= SelfCharacter->ReleaseRange)
	// 			{
	// 				return true;
	// 			}
	// 		}
	// 		break;
	// }
	
	return false;
}

void UAutoChessAIBBContext::SetTargetAI(ATheOneAutoChessAIController* InTargetAI)
{
	// auto Self = Cast<AHexTheOneAIController>(GetOuter());
	// check(Self);
	// Self->GetBlackboardComponent()->ClearValue(TargetIsDeadKey);
	// if (TargetAI != nullptr)
	// {
	// 	auto OldCharacter = Cast<ATheOneCharacterBase>(TargetAI->GetPawn());
	// 	if (OldCharacter)
	// 	{
	// 		OldCharacter->OnceOnDead.Remove(TargetDeadHandle);
	// 	}
	// }
	//
	// TargetAI = InTargetAI;
	//
	// if (InTargetAI != nullptr)
	// {
	// 	auto TargetCharacter = Cast<ATheOneCharacterBase>(InTargetAI->GetPawn());
	// 	if (TargetCharacter)
	// 	{
	// 		TargetDeadHandle = TargetCharacter->OnceOnDead.AddUObject(
	// 			this, &UAutoChessAIBBContext::OnTargetDead);
	// 	}
	// }
}

void UAutoChessAIBBContext::OnTargetDead()
{
	auto Self = Cast<ATheOneAutoChessAIController>(GetOuter());
	check(Self);
	Self->GetBlackboardComponent()->SetValueAsBool(TargetIsDeadKey, true);
}
