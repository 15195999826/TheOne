#pragma once
#include "CoreMinimal.h"
#include "TheOneAdditionMesh.generated.h"

USTRUCT(BlueprintType)
struct FTheOneAdditionMesh
{
	GENERATED_BODY()
	
	FTheOneAdditionMesh(): Mesh(nullptr), SocketName(NAME_None)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AttachToOutlook = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FollowRootAnimation = false;
};
