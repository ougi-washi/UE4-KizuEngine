// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Core/Combat/KizuCombat.h"
#include "AnimNotify_KSpawnBuff.generated.h"

class AKBuff;

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Spawn Buff (Kizu)"))
class KIZUENGINE_API UAnimNotify_KSpawnBuff : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:

	UPROPERTY(EditAnywhere, Category = "Kizu")
	TSubclassOf<AKBuff> BuffClass;

	UPROPERTY(EditAnywhere, Category = "Kizu")
	EKBuffApplication BuffApplication;
};
