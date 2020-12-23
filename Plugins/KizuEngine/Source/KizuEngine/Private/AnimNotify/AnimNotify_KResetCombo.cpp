
#include "AnimNotify/AnimNotify_KResetCombo.h"
#include "Core/KCharacter.h"
#include "KizuEngine.h"

void UAnimNotify_KResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AKCharacter* KCharacter = Cast<AKCharacter>(MeshComp->GetOwner()))
		KCharacter->ComboCounter = 0;
	else UE_LOG(LogKizu, Warning, TEXT("Cannot use the Reset Combo AnimNotify on a non-KCharacter."));
}
