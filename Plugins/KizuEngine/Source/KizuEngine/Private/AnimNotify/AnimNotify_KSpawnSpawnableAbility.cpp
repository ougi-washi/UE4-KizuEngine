// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_KSpawnSpawnableAbility.h"
#include "Core/KCharacter.h"
#include "Core/Combat/KSpawnableAbility.h"
#include "KizuEngine.h"

void UAnimNotify_KSpawnSpawnableAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		if (KCharacter->IsLocallyControlled())
			KCharacter->ServerSpawnActor(SpawnableAbilityToSpawn, KCharacter->GetActorTransform());
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot use the Spawn SpawnableAbility AnimNotify on a non-KCharacter."));
}
