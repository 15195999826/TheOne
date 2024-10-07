#pragma once

#include "CoreMinimal.h"

#include "TheOneCharacterUnique.generated.h"

USTRUCT(BlueprintType)
struct FTheOneCharacterUnique
{
	GENERATED_BODY()

	FTheOneCharacterUnique(): Flag(0)
	{
	}

	// 用于查询对应的是哪个场景中的角色
	uint32 Flag;

	UPROPERTY(BlueprintReadOnly)
	FText Name;

	// 队伍总共两排， 每排站9个人
	// 于是 0-8是第一排， 9-17是第二排
	// 第三排为后援， 18-26
	int32 TeamPosition = 0;

	// 其它角色属性的随机
	// Todo: 
};
