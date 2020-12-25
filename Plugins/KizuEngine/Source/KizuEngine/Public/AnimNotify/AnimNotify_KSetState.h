

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_KSetState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Set State (Kizu)"))
class KIZUENGINE_API UAnimNotify_KSetState : public UAnimNotify
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Kizu")
	FString NewState = "Hitting";

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
