// Copyright (c) 2019 Nexus Division. All rights reserved.


#include "Core/Combat/KProjectileAbilitySpawner.h"
#include "Core/KCharacter.h"

void AKProjectileAbilitySpawner::OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionBeginOverlap_Native(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Affect all environment besides pawns
	if (!OtherActor->GetClass()->IsChildOf(APawn::StaticClass()))
	{
		if (AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner()))
		{
			if (OwnerCharacter->IsLocallyControlled())
			{
				TriggerDestroyTimer(SpawnableAbilityData.DestroyTimer);
				FSpawnableAbilitySpawnParams SpawnParams;
				SpawnParams.Transform = FTransform(FRotator(0, 0, 0), GetActorLocation(), GetActorScale());
				SpawnParams.bInitizalizeMobility = false;
				SpawnParams.InitialDirection = FVector(0, 0, 0);
				SpawnParams.TargetActor = nullptr;
				OwnerCharacter->ServerSpawnSpawnableAbility(AbilityToSpawn, SpawnParams);
			}
		}
	}
}
