
#include "AnimNotify/AnimNotify_KSetState.h"
#include "Core/KCharacter.h"

void UAnimNotify_KSetState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner())) {
		if (KCharacter->IsLocallyControlled())
			KCharacter->ServerSetCurrentState(NewState);
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot use the Set State AnimNotify on a non-KCharacter."));
}
