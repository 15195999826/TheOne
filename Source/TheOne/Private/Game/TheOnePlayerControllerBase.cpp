// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TheOnePlayerControllerBase.h"

#include "RegisterInputActionTask.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "AbilitySystem/Abilities/TheOneGeneralGA.h"
#include "AI/TheOneAICommandInterface.h"
#include "Character/TheOneCharacterBase.h"
#include "Game/CursorTraceInterface.h"
#include "Game/TheOneEventSystem.h"
#include "Widgets/SWidget.h"
// 注意，这个Include不能删除
#include "Widgets/SViewport.h"
#include "Subsystems/TheOneContextSystem.h"

void ATheOnePlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	bIsPawnImplementICursorTrace = GetPawn()->GetClass()->ImplementsInterface(UCursorTraceInterface::StaticClass());

	auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
	EventSystem->UseAbilityCommand.AddDynamic(this, &ATheOnePlayerControllerBase::ReceiveUseAbilityCommand);
	EventSystem->OnAbilityCommandCanceled.AddDynamic(this, &ATheOnePlayerControllerBase::HideReleaseDistanceTips);
	EventSystem->OnAbilityCommandFinished.AddDynamic(this, &ATheOnePlayerControllerBase::HideReleaseDistanceTips);
}

void ATheOnePlayerControllerBase::DeselectCharacter()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->OnDeselected();
		SelectedCharacter = nullptr;
	}
}

void ATheOnePlayerControllerBase::GeneralOnHitCharacter(ATheOneCharacterBase* HitCharacter)
{
	if (bIsPawnImplementICursorTrace)
	{
		ICursorTraceInterface::Execute_OnHitActor(GetPawn(), HitCharacter);
	}
}

void ATheOnePlayerControllerBase::GeneralOnHitNone()
{
	
}

void ATheOnePlayerControllerBase::BP_OnHitGround_Implementation(const FVector& HitLocation, bool bIsRightClick, bool bIsLeftClick,
	bool CanWalk)
{
}

void ATheOnePlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 自定义的LineTraceMultiByChannel， 暂时不使用
	// ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	// bool bHit = false;
	// if (LocalPlayer && LocalPlayer->ViewportClient)
	// {
	// 	FVector2D MousePosition;
	// 	if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
	// 	{
	// 		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false );
	// 		if (GetHUD() != NULL && GetHUD()->GetHitBoxAtCoordinates(MousePosition, true))
	// 		{
	// 			return;
	// 		}
	//
	// 		FVector WorldOrigin;
	// 		FVector WorldDirection;
	// 		if (UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, WorldOrigin, WorldDirection) == true)
	// 		{
	// 			HitResults.Empty();
	// 			bHit = GetWorld()->LineTraceMultiByChannel(HitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ECC_Visibility, CollisionQueryParams, ResponseParam);
	// 		}
	// 	}
	// }
	//
	// if(!bHit)	//If there was no hit we reset the results. This is redundant but helps Blueprint users
	// {
	// 	return;
	// }

	// 检查鼠标是否在UI上
	TSharedPtr ViewPort = FSlateApplication::Get().GetGameViewport();

	if (ViewPort.IsValid())
	{
		FWidgetPath WidgetUnderMouse = FSlateApplication::Get().LocateWindowUnderMouse(FSlateApplication::Get().GetCursorPos() , FSlateApplication::Get().GetInteractiveTopLevelWindows(), true);
		
		OverWidget = !(WidgetUnderMouse.IsValid() &&  WidgetUnderMouse.GetLastWidget() == ViewPort.ToSharedRef());
	}
	
	
	bool bIsRightClick = WasInputKeyJustPressed(EKeys::RightMouseButton);
	bool bIsLeftClick = WasInputKeyJustPressed(EKeys::LeftMouseButton);
	FHitResult HitResult;
	bool HitGround = false;
	FVector HitGroundLocation;
	ATheOneCharacterBase* HitCharacter = nullptr;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit)
	{
		auto HitActor = HitResult.GetActor();
		if (HitActor)
		{
			if (HitResult.GetActor()->ActorHasTag("Ground"))
			{
				GeneralOnHitGround(HitResult.Location, HitGroundLocation);
				HitGround = true;
			}
			else if (HitResult.GetActor()->ActorHasTag("Character"))
			{
				HitCharacter = Cast<ATheOneCharacterBase>(HitActor);

				// 通用命中角色时调用
				if (HitCharacter)
				{
					GeneralOnHitCharacter(HitCharacter);
				}
			}
		}
	}

	if (!HitGround && HitCharacter == nullptr)
	{
		GeneralOnHitNone();
	}

	switch (CursorState) {
		case ETheOneCursorState::Normal:
			{
				if (HitGround)
				{
					BP_OnHitGround(HitGroundLocation, bIsRightClick, bIsLeftClick,
					               bIsLeftClick ? CanWalk(HitGroundLocation) : false);
				}
				else if (HitCharacter)
				{
					BP_OnHitActor(HitCharacter, bIsRightClick);

					// 角色的射线追踪
					if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
					{
						auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
						auto ContextSystem = GetWorld()->GetSubsystem<UTheOneContextSystem>();
						if (HitCharacter)
						{
							DeselectCharacter();
							ContextSystem->WeakSelectedCharacter = HitCharacter;
							SelectedCharacter = HitCharacter;
							SelectedCharacter->OnSelected();
							EventSystem->OnSelectCharacter.Broadcast(SelectedCharacter);
						}
						else
						{
							ContextSystem->WeakSelectedCharacter = nullptr;
							DeselectCharacter();
							EventSystem->OnDeSelectCharacter.Broadcast();
						}
					}
				}
			}
			break;
		case ETheOneCursorState::SelectActor:
			{
				if (bIsRightClick)
				{
					auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
					EventSystem->OnAbilityCommandCanceled.Broadcast();
					ChangeCursorState(ETheOneCursorState::Normal);
				}
				else if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
				{
					auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
					if (HitCharacter && HasUseAbilityCommandCache)
					{
						bool SuccessSelect = false;
						switch (SelectActorType) {
							case ETheOneSelectActorType::Any:
								SuccessSelect = true;
							break;
							case ETheOneSelectActorType::Ally:
								if (IInHexActorInterface::Execute_GetCamp(SelectedCharacter) == IInHexActorInterface::Execute_GetCamp(HitCharacter))
								{
									SuccessSelect = true;
								}
								break;
							case ETheOneSelectActorType::Enemy:
								if (IInHexActorInterface::Execute_GetCamp(SelectedCharacter) != IInHexActorInterface::Execute_GetCamp(HitCharacter))
								{
									SuccessSelect = true;
								}
								break;
							case ETheOneSelectActorType::Special:
								break;
						}

						
						if (SuccessSelect)
						{
							check(SelectedCharacter);
							ITheOneAICommandInterface::Execute_CommitAbility(
								SelectedCharacter->GetController(), PayloadCache, HitCharacter,
								HitCharacter->GetActorLocation(), ReleaseDistanceCache);
							
							EventSystem->OnAbilityCommandFinished.Broadcast();  

							HasUseAbilityCommandCache = false;
							ChangeCursorState(ETheOneCursorState::Normal);
						}
						else
						{
							EventSystem->OnAbilityCommandErrorTarget.Broadcast();
						}
					}
					else
					{
						EventSystem->OnAbilityCommandErrorTarget.Broadcast();
					}
				}
			}
			break;
		case ETheOneCursorState::SelectGround:
			if (bIsRightClick)
			{
				auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
				EventSystem->OnAbilityCommandCanceled.Broadcast();
				ChangeCursorState(ETheOneCursorState::Normal);
			}
			else if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
			{
				auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
				// Todo: 如果点击到的Actor， 那么可以将位置设置为Actor位置？ Or 这次点击在做涉嫌检测时忽略Actor?
				if (HitGround)
				{
					if (HasUseAbilityCommandCache)
					{
						check(SelectedCharacter);
						ITheOneAICommandInterface::Execute_CommitAbility(
							SelectedCharacter->GetController(), PayloadCache, nullptr, HitGroundLocation, ReleaseDistanceCache);
						EventSystem->OnAbilityCommandFinished.Broadcast();
							
						HasUseAbilityCommandCache = false;
						ChangeCursorState(ETheOneCursorState::Normal);
					}
				}
			}
			break;
	}
}

void ATheOnePlayerControllerBase::CursorTrace()
{
	// if (!bIsPawnImplementICursorTrace && !bControlSelectedCharacter)
	// {
	// 	return;
	// }
	
	FVector Start = PlayerCameraManager->GetCameraLocation();
	FVector2D ScreenPosition;
	FVector End;
	
	if (GetMousePosition(ScreenPosition.X, ScreenPosition.Y))
	{
		FVector WorldPosition, WorldDirection;
		if (DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldPosition, WorldDirection))
		{
			End = WorldPosition + WorldDirection * 1000;
		}
	}
	HitResults.Empty();
	// 追踪地面， 地面被设置在了ECC_Visibility， 而Pawn默认射线检测是在ECC_Camera
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam, ResponseParam);
	bool bHitGround = false;
	FHitResult CursorHit;
	for (const FHitResult& HitResult : HitResults)
	{
		
		auto HitComponent = HitResult.GetComponent();
		if (HitComponent && HitComponent->ComponentHasTag("Ground"))
		{
			// 先检查Component是否包含Tag
			bHitGround = true;
			CursorHit = HitResult;
			break;
		}
		
		auto HitActor = HitResult.GetActor();
		if (HitActor && HitResult.GetActor()->ActorHasTag("Ground"))
		{
			// 检查Tag是否包含Ground
			bHitGround = true;
			CursorHit = HitResult;
			break;
		}
	}
}

void ATheOnePlayerControllerBase::RegisterInputAction(URegisterInputActionTask* InTask)
{
	if (InputComponent == nullptr)
	{
		UE_LOG(LogTheOne, Error, TEXT("InputComponent is nullptr"));
		return;
	}

	if (InputID2IndexMap.Contains(InTask->TaskID))
	{
		UE_LOG(LogTheOne, Error, TEXT("InputActionConfig.StrID is already registered"));
		return;
	}

	
	FInputKeyBinding KB(InTask->Key, InTask->InputEvent);

	KB.KeyDelegate.GetDelegateForManualSet().BindLambda([InTask]()
	{
		InTask->Delegate.Execute();
	});

	InputComponent->KeyBindings.Emplace(MoveTemp(KB));
	InputID2IndexMap.Add(InTask->TaskID, InputComponent->KeyBindings.Num() - 1);
	InputTasks.Add(InTask);
}

void ATheOnePlayerControllerBase::UnregisterInputAction(const int& InTaskID)
{
	if (InputID2IndexMap.Contains(InTaskID))
	{
		int Index = InputID2IndexMap[InTaskID];
		InputComponent->KeyBindings.RemoveAt(Index);
		InputID2IndexMap.Remove(InTaskID);
		// 倒叙删除
		for (int i = InputTasks.Num() - 1; i >= 0; i--)
		{
			if (InputTasks[i]->TaskID == InTaskID)
			{
				InputTasks.RemoveAt(i);
				break;
			}
		}
	}
}

void ATheOnePlayerControllerBase::GeneralOnHitGround(const FVector& InHitLocation, FVector& OutGroundLocation)
{
	OutGroundLocation = InHitLocation;
	if (bIsPawnImplementICursorTrace)
	{
		ICursorTraceInterface::Execute_OnHitGround(GetPawn(), InHitLocation);
	}
}

void ATheOnePlayerControllerBase::ReceiveUseAbilityCommand(const FTheOneUseAbilityCommandPayload& Payload)
{
	HasUseAbilityCommandCache = false;
	// Todo:  检查这个道具是否需要装备在角色身上才能用， 目前没有全局使用的道具
	//  目前仅支持，1，2，3三个技能的使用
	if (SelectedCharacter == nullptr)
	{
		return;
	}
	auto InUseAbilityCommandType = Payload.CommandType;
	FTheOneAbilityConfig* AbilityConfig = nullptr;
	// Todo: 根据当前技能的使用方式， 决定目标
	switch (InUseAbilityCommandType) {
		case ETheOneUseAbilityCommandType::UseItem:
			break;
		case ETheOneUseAbilityCommandType::UseWeaponAbility:
			{
				UTheOneGeneralGA* Ability = nullptr;

				// Todo: 技能释放
				// if (IntPayload == 1)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityA.Get();
				// }
				// else if (IntPayload == 2)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityB.Get();
				// }
				// else if (IntPayload == 3)
				// {
				// 	Ability = SelectedCharacter->WeakWeaponAbilityC.Get();
				// }

				// 因为会默认给与空技能，因此不存在空指针的情况
				check(Ability);
				
				AbilityConfig = Ability->GetAbilityRowHandle().GetRow<FTheOneAbilityConfig>("ReceiveUseAbilityCommand");
			}
			break;
		default:
			UE_LOG(LogTheOne, Error, TEXT("UnSolved UseAbilityCommandType: %d"), InUseAbilityCommandType);
			break;
	}

	
	if (AbilityConfig)
	{
		switch (AbilityConfig->TargetType) {
			case ETheOneAbilityReleaseTarget::None:
				ITheOneAICommandInterface::Execute_CommitAbility(SelectedCharacter->GetController(), PayloadCache, nullptr, SelectedCharacter->GetActorLocation(), AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Enemy:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Enemy, AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Ally:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Ally, AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::AnyActor:
				HasUseAbilityCommandCache = true;
				// 改变Cursor样式， 等待玩家点击
				ChangeCursorState(ETheOneCursorState::SelectActor, ETheOneSelectActorType::Any, AbilityConfig->ReleaseDistance);
				break;
			case ETheOneAbilityReleaseTarget::Grid:
				HasUseAbilityCommandCache = true;
				ChangeCursorState(ETheOneCursorState::SelectGround, ETheOneSelectActorType::Any, AbilityConfig->ReleaseDistance);
				break;
		}
		
		if (HasUseAbilityCommandCache)
		{
			ReleaseDistanceCache = AbilityConfig->ReleaseDistance;
			PayloadCache = Payload;
			ShowReleaseDistanceTips();
			auto EventSystem = GetWorld()->GetSubsystem<UTheOneEventSystem>();
			EventSystem->OnAbilityCommandWaiting.Broadcast(Payload);
		}
	}
}

void ATheOnePlayerControllerBase::ShowReleaseDistanceTips()
{
}

void ATheOnePlayerControllerBase::HideReleaseDistanceTips()
{
}

void ATheOnePlayerControllerBase::ChangeCursorState(ETheOneCursorState InState, ETheOneSelectActorType InSelectActorType, int InReleaseDistance)
{
	CursorState = InState;
	SelectActorType = InSelectActorType;
	OnCursorStateChange(InState, InSelectActorType, InReleaseDistance);
}

