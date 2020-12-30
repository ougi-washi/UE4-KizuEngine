// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_KSpawnSpawnableAbility.generated.h"

class AKSpawnableAbility;

/**
 * 
 */

UCLASS(Blueprintable, meta = (DisplayName = "Spawn SpawnableAbility (Kizu)"))
class KIZUENGINE_API UAnimNotify_KSpawnSpawnableAbility : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Kizu")
	TSubclassOf<AKSpawnableAbility> SpawnableAbilityToSpawn;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
