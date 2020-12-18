// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_KSpawnSpell.h"
#include "Core/KCharacter.h"
#include "Core/Combat/KSpell.h"

void UAnimNotify_KSpawnSpell::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		KCharacter->ServerSpawnActor(SpellToSpawn, KCharacter->GetActorTransform());
	}
}
