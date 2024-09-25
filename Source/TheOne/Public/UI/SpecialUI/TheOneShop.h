// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheOneShop.generated.h"

class UTheOneItemUserWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class THEONE_API UTheOneShop : public UUserWidget
{
	GENERATED_BODY()

public:
	
public:
	/**
	 * @brief 初始化商店, 生成所有通用道具
	 */
	void Init();

	UFUNCTION(BlueprintCallable)
	/**
	 * @brief 刷新珍藏，包括武器、让武器进阶的特殊道具等
	 */
	void RefreshTreasure();
};
