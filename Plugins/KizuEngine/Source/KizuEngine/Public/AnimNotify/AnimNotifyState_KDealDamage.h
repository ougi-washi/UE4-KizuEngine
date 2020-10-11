// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_KDealDamage.generated.h"

UENUM(BlueprintType)
enum EValueSource
{
	Static,
	Dynamic,
	Custom
};

class AKCharacter;

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Kizu Deal Damage"))
class KIZUENGINE_API UAnimNotifyState_KDealDamage : public UAnimNotifyState
{

	GENERATED_BODY()

protected:

	TArray<UActorComponent*> CollisionComponents;
	TArray<AActor*> DamagedActors;
	AKCharacter* OwnerCharacter;
	float FinalDamage = 0;

public:

	/** Tag of the collision component to trace from in order to deal damage. This component should have a Primitive Component as a parent */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta=(Tooltip = "Tag of the collision component to trace from in order to deal damage"))
	FName CollisionComponentTag = "Damage";
	/** Choose the source of the damage value */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Choose the source of the damage value"))
	TEnumAsByte<EValueSource> ValueSource = EValueSource::Static;
	/** Damage value to deal */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Damage value to deal, constant static value that doesn't change"))
	float DamageValue = 20.f;
	/** Resource name to use as a Base damage */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Resource name to use as a Base damage"))
	FString ResourceName = "None";
	/** Percentage value of the Base damage */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Percentage value of the Base damage"))
	float PercentageValue = 100.f;
	/** Damage Type */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Damage Type"))
	TSubclassOf<UDamageType> DamageType;
	/** Custom damage ID. Make sure to add this ID to the stack of the custom damages in the KCharacter before executing this notify */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Custom damage ID. Make sure to add this ID to the stack of the custom damages in the KCharacter executing this notify"))
	FString CustomDamageID;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;

	// Make sure you call this when OwnerCharacter is valid
	float CalculateDamage();
	// Make sure you call this when OwnerCharacter is valid
	void DamageActors(TArray<AActor*> &Actors);

	// Note CanEditChange is only available when compiling with the editor.
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

};
