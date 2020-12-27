// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_KSpawnSpell.h"
#include "Core/KCharacter.h"
#include "Core/Combat/KSpell.h"
#include "KizuEngine.h"

void UAnimNotify_KSpawnSpell::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		if (KCharacter->IsLocallyControlled())
			KCharacter->ServerSpawnActor(SpellToSpawn, KCharacter->GetActorTransform());
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot use the Spawn Spell AnimNotify on a non-KCharacter."));
}
