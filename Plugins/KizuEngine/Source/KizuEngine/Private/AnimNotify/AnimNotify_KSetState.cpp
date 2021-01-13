
#include "AnimNotify/AnimNotify_KSetState.h"
#include "Core/KCharacter.h"

void UAnimNotify_KSetState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		if (KCharacter->IsLocallyControlled())
			KCharacter->SetCurrentStateFast(NewState);
	}
}
