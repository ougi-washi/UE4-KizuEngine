// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_KSpawnSpawnableAbility.h"
#include "Core/KCharacter.h"
#include "Core/Combat/KSpawnableAbility.h"
#include "KizuEngine.h"


UAnimNotify_KSpawnSpawnableAbility::UAnimNotify_KSpawnSpawnableAbility() : Super()
{
	bInitializeMovement = true;
	bUseCrosshair = true;
}

void UAnimNotify_KSpawnSpawnableAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		if (KCharacter->IsLocallyControlled()) {
			float ResultRange;
			if (bUseResourceAsRange && KCharacter->GetResourceCurrentValue(ResourceName, ResultRange))
				 ResultRange = ResultRange * ResourcePercentage / 100.f;
			else ResultRange = TargettingRange;
			KCharacter->SpawnSpawnableAbility_Replicated(SpawnableAbilityToSpawn, bInitializeMovement, bUseCrosshair, SocketName, ResultRange, CollisionChannel);
		}
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot use the Spawn SpawnableAbility AnimNotify on a non-KCharacter."));
}
