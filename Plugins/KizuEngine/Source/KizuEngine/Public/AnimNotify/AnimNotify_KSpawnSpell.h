// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_KSpawnSpell.generated.h"

class AKSpell;

/**
 * 
 */

UCLASS(Blueprintable, meta = (DisplayName = "Spawn Spell (Kizu)"))
class KIZUENGINE_API UAnimNotify_KSpawnSpell : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Kizu")
	TSubclassOf<AKSpell> SpellToSpawn;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
