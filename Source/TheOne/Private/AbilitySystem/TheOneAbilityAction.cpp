#include "AbilitySystem/TheOneAbilityAction.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ITheOneCharacterSpawnActor.h"
#include "TheOneBlueprintFunctionLibrary.h"
#include "TheOneLogChannels.h"
#include "AbilitySystem/TheOneAttributeSet.h"
#include "AbilitySystem/TheOneLifeAttributeSet.h"
#include "AbilitySystem/Abilities/TheOneDataDrivePassiveGA.h"
#include "AbilitySystem/Abilities/TheOneGameplayAbility.h"
#include "Actor/TheOneProjectileActor.h"
#include "Character/TheOneCharacterBase.h"
#include "Development/TheOneGeneralSettings.h"
#include "Game/TheOneBattle.h"
#include "Subsystems/TheOneContextSystem.h"

void FTheOneAbilityAction::DoAction(AActor* ActionExecutor, const FTheOneAbilityTargetSelector& InTargetSelector,
                                    int Level, UTheOneGameplayAbility* FromAbility, FActiveGameplayEffect* ActionSourceGE, bool DrawDebug)
{
	// Todo: 保证ActionExecutor拥有ActionSourceGE
	// Todo: Caster 可以为投射物等， 通过接口可以获取真正的施法者
	
	// 查询目标
	const TArray<AActor*>& Targets = GetTargets(ActionExecutor, InTargetSelector, FromAbility, DrawDebug);

	switch (ActionData.ActionType)
	{
		case ETheOneAbilityActionType::Invalid:
			UE_LOG(LogTheOne, Error, TEXT("Actor %s, Ability: %s, ActionData.ActionType is Invalid"), *ActionExecutor->GetName(), *FromAbility->GetName());
			break;
		case ETheOneAbilityActionType::Damage:
			{
				auto SourceActor = ActionExecutor;
				if (ActionExecutor->Implements<UTheOneCharacterSpawnActor>())
				{
					SourceActor = Cast<ITheOneCharacterSpawnActor>(ActionExecutor)->GetSourceCharacter();
				}

				FString Expression = "0";
				
				if (ActionData.Expressions.Num() > 0)
				{
					Expression = ActionData.Expressions[FMath::Clamp(Level - 1, 0, ActionData.Expressions.Num() - 1)];
				}
				// 是否完全等于ATK
				bool IsUseATK = Expression.Equals("ATK");
				// Todo: 可以根据配置决定伤害攻击的位置

				auto ContextSystem = SourceActor->GetWorld()->GetSubsystem<UTheOneContextSystem>();
				for (auto Target : Targets)
				{
					auto Damage = RequireActionMathExpression(Expression, SourceActor, Target, ContextSystem->Battle->GetRandomStream(), ActionSourceGE);
					DoActionDamageInternal(SourceActor, Target, Damage, FromAbility, IsUseATK);
				}
			}
			break;
		case ETheOneAbilityActionType::SpawnProjectile:
			{
				auto ProjectileTable = GetDefault<UTheOneDataTableSettings>()->ProjectileTable.LoadSynchronous();
				auto SpawnRow = ProjectileTable->FindRow<FTheOneProjectileConfig>(ActionData.ProjectileRowName, "DoAction");
				if (SpawnRow == nullptr)
				{
					break;
				}

				FTransform SpawnTransform = ActionExecutor->GetActorTransform();
				// 总是从ActionExecutor的朝向发射的，只从Socket获取Location
				if (ActionData.SpawnSocketName != NAME_None)
				{
					FVector SocketLocation;
					if (UTheOneBlueprintFunctionLibrary::TryFindSocketLocation(
						ActionExecutor, ActionData.SpawnSocketName, SocketLocation))
					{
						SpawnTransform.SetLocation(SocketLocation);
					}
				}

				FVector ForwardVector = ActionExecutor->GetActorForwardVector();

				// 计算偏移量
				FVector Offset = ForwardVector * ActionData.SpawnOffset.X + ActionExecutor->GetActorRightVector() * ActionData.SpawnOffset.Y + ActionExecutor->GetActorUpVector() * ActionData.SpawnOffset.Z;


				// 相对角色朝向，进行ActionData.SpawnOffset的偏移
				SpawnTransform.SetLocation(SpawnTransform.GetLocation() + Offset);
				for (auto Target : Targets)
				{
					FActorSpawnParameters SpawnInfo;
					SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					SpawnInfo.OverrideLevel = ActionExecutor->GetLevel();
					SpawnInfo.ObjectFlags |= RF_Transient;
					// 仅蓝图类支持
					SpawnInfo.bDeferConstruction = true;

					ATheOneProjectileActor* NewSpawnActor = ActionExecutor->GetWorld()->SpawnActor<ATheOneProjectileActor>(
						SpawnRow->ProjectileClass, SpawnTransform, SpawnInfo);
					if (NewSpawnActor != nullptr)
					{
						TArray<FTheOneAbilityActionData*> HitActions;
						if (ActionData.HitActionKeys.Num() > 0)
						{
							for (const auto& HitActionKey : ActionData.HitActionKeys)
							{
								HitActions.Add(&ForBindActionParam[HitActionKey]);
							}
						}
						// Todo: 根据配置可能需要传入不同的Location, 比如目标格子位置
						NewSpawnActor->SetUp(SpawnRow,
						                     ActionExecutor, FromAbility,
						                     Target, Target->GetActorLocation(),
						                     ActionData.FlyType, ActionData.FlySpeed,
						                     HitActions
						);
						NewSpawnActor->FinishSpawning(SpawnTransform);
					}
				}
			}
			break;
		case ETheOneAbilityActionType::AddBuff:
			break;
	}
}

void FTheOneAbilityAction::DoActionDamageInternal(AActor* SourceActor, AActor* TargetActor, float InDamage,
	const UTheOneGameplayAbility* FromAbility, bool IsUseATK) const
{
	const auto SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	const auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!TargetASC)
	{
		return;
	}
	
	auto EffectContext = SourceASC->MakeEffectContext();
	EffectContext.SetAbility(FromAbility);
	EffectContext.AddSourceObject(SourceActor);
	const auto SpecHandle = SourceASC->MakeOutgoingSpec(GetDefault<UTheOneGeneralSettings>()->DamageEffect, 1, EffectContext);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ActionData.DamageTag, InDamage);
	if (ActionData.DamageTag == TheOneGameplayTags::SetByCaller_Damage_Melee)
	{
		int Surrounded = UTheOneBlueprintFunctionLibrary::GetSurroundingEnemyCount(TargetActor);

		if (Surrounded > 1)
		{
			// 每个包围目标的单位提供5点近战水平
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Damage_SurrondExtra, Surrounded * 5);
		}
		else
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TheOneGameplayTags::SetByCaller_Damage_SurrondExtra, 0);
		}
	}
	
	// Add Attack Position Dynamic Tag
	FGameplayTag AttackPositionTag;
	if (!ActionData.DamagePositionTag.IsValid())
	{
		// 随机一个位置
		auto RandomStream = SourceActor->GetWorld()->GetSubsystem<UTheOneContextSystem>()->Battle->GetRandomStream();
		auto RandomValue = RandomStream.FRand();
		float HitHeadRate = 0.f;
		if (SourceASC)
		{
			HitHeadRate = SourceASC->GetNumericAttribute(UTheOneAttributeSet::GetHeadshotRateAttribute());
		}
		
		if (RandomValue > HitHeadRate)
		{
			AttackPositionTag = TheOneGameplayTags::SetByCaller_DamagePosition_Body;
		}
		else
		{
			AttackPositionTag = TheOneGameplayTags::SetByCaller_DamagePosition_Head;
		}
	}
	else
	{
		AttackPositionTag = ActionData.DamagePositionTag;
	}

	SpecHandle.Data.Get()->AddDynamicAssetTag(AttackPositionTag);

	// Todo: 增加Tag， 是否正在使用ATK计算伤害， 是的话才使用公式
	if (IsUseATK)
	{
		SpecHandle.Data.Get()->AddDynamicAssetTag(TheOneGameplayTags::Damage_Calc_ByATK);
	}

	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<AActor*> FTheOneAbilityAction::GetTargets(AActor* ActionExecutor,
	const FTheOneAbilityTargetSelector& InTargetSelector, UTheOneGameplayAbility* FromAbility, bool DrawDebug)
{
	TArray<AActor*> Ret;
	switch (InTargetSelector.TargetSelectorType)
	{
		case ETheOneTargetSelectorType::BBTarget:
			{
				if (ActionExecutor->Implements<UTheOneBattleInterface>())
				{
					Ret.Add(ITheOneBattleInterface::Execute_GetTargetActor(ActionExecutor));
				}
				else
				{
					auto SpawnedActor = Cast<ITheOneCharacterSpawnActor>(ActionExecutor);
					if (SpawnedActor)
					{
						Ret.Add(ITheOneBattleInterface::Execute_GetTargetActor(SpawnedActor->GetSourceCharacter()));
					}
				}
			}
			break;
		case ETheOneTargetSelectorType::HitBox:
			{
				for (const auto& HitBox : InTargetSelector.HiBoxes)
				{
					// Todo: 根据 HitBox 的HitBoxPointType 来设置 HitBox 的中心
					auto Result = UTheOneBlueprintFunctionLibrary::GetHitBoxHitResult(
						ActionExecutor,
						ActionExecutor->GetActorLocation(),
						ActionExecutor->GetActorForwardVector(),
						ActionExecutor->GetActorRotation(),
						HitBox.HitBoxType,
						HitBox.HitMultiple,
						HitBox.BoxSize,
						HitBox.SectorAngle,
						HitBox.Radius,
						HitBox.AboveGround,
						HitBox.StartDistance,
						HitBox.EndDistance,
						HitBox.IgnoreSelf,
						DrawDebug);

					for (auto Actor : Result)
					{
						Ret.Add(Actor);
					}
				}
			}
			break;
		case ETheOneTargetSelectorType::FromInterface:
			{
				if (auto HitTarget = Cast<ITheOneHitTarget>(ActionExecutor))
				{
					auto Target = HitTarget->GetHitTarget();
					if (Target)
					{
						Ret.Add(Target);
					}
				}
			}
			break;
		case ETheOneTargetSelectorType::Special:
			// Todo: Special
			break;
		case ETheOneTargetSelectorType::Invalid:
			break;
		case ETheOneTargetSelectorType::TriggerActor:
			{
				if (auto PassiveGA = Cast<UTheOneDataDrivePassiveGA>(FromAbility))
				{
					Ret.Add(PassiveGA->GetCurrentEventTriggerActor());
				}
			}
			break;
	}

	return Ret;
}

float FTheOneAbilityAction::RequireActionMathExpression(const FString& MathExpression,
	const AActor* ActionExecutor, const AActor* TargetActor, const FRandomStream& RandomStream, const FActiveGameplayEffect* InSourceGE)
{
	FBasicMathExpressionEvaluator Evaluator;
	FString Expression = MathExpression; // copy

	bool GetATK = false;
	auto MinATK = UAbilitySystemBlueprintLibrary::GetFloatAttribute(ActionExecutor, UTheOneAttributeSet::GetMinAttackAttribute(),GetATK);
	auto MaxATK = UAbilitySystemBlueprintLibrary::GetFloatAttribute(ActionExecutor, UTheOneAttributeSet::GetMaxAttackAttribute(), GetATK);
	auto RDATK = RandomStream.FRandRange(MinATK, MaxATK);

	bool GetMaxHP = false;
	auto MaxHealth = UAbilitySystemBlueprintLibrary::GetFloatAttribute(TargetActor, UTheOneLifeAttributeSet::GetMaxHealthAttribute(), GetMaxHP);

	// Todo: 保存一个属性CacheMap， 先检查是否相等，再判断要不要重新计算
	
	//Todo: 填充各种属性
 
	auto StackCount = 0;
	if (InSourceGE != nullptr)
	{
		StackCount = InSourceGE->Spec.GetStackCount();
	}
	
	Expression = Expression.Replace(TEXT("MaxHP"), *FString::Printf(TEXT("%f"), MaxHealth));
	Expression = Expression.Replace(TEXT("ATK"), *FString::Printf(TEXT("%f"), RDATK));
	Expression = Expression.Replace(TEXT("BuffStack"), *FString::Printf(TEXT("%d"), StackCount));
 
	TValueOrError<double, FExpressionError> Result = Evaluator.Evaluate(Expression.GetCharArray().GetData());
	if (Result.IsValid())
	{
		return Result.GetValue();
	}
	UE_LOG(LogTemp, Error, TEXT("[RequireActionMathExpression] Failed to evaluate the expression: %s"), *Result.GetError().Text.ToString());
 	
	return 0.f;
}


