// Copyright (c) 2019 Nexus Division. All rights reserved.


#include "AnimNotify/AnimNotify_KSpawnBuff.h"
#include "Core/KCharacter.h"

void UAnimNotify_KSpawnBuff::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && BuffClass)
	{
		if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner()))
		{
			if (KCharacter->IsLocallyControlled())
			{
				KCharacter->SpawnBuff(BuffClass, BuffApplication);
			}
		}
	}
	
}
