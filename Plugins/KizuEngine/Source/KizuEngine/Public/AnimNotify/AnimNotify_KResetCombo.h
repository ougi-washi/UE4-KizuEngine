

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_KResetCombo.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Reset Combo (Kizu)"))
class KIZUENGINE_API UAnimNotify_KResetCombo : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
