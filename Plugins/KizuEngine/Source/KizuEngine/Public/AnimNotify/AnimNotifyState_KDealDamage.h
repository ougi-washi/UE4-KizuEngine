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
class UDataTable;

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Deal Damage (Kizu)"))
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
	/** Whether it or not will ignore the owning Character */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Whether or not it will ignore the owning Character."))
	uint8 bIgnoreSelf : 1;
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
	/** Custom damage ID. Make sure to add this ID to the stack of the custom damages in the KCharacter before executing this notify via (Add Custom Damage) node */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Custom damage ID. Make sure to add this ID to the stack of the custom damages in the KCharacter before executing this notify via (Add Custom Damage) node. "))
	FString CustomDamageID;
	/** Whether send a reaction to the targeted character or not. (It won't receive a reaction unless it's a character) */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (Tooltip = "Whether send a reaction to the targeted character or not. (It won't receive a reaction unless it's a character)"))
	uint8 bSendReaction : 1;
	/** Data table to use as a source of the reaction */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "bSendReaction", Tooltip = "Data table to use as a source of the reaction"))
	UDataTable* ReactionDataTable;
	/** Data table Row Name for the reaction */
	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "bSendReaction", Tooltip = "Data table Row Name for the reaction"))
	FString ReactionRowName = "None";
	
	UAnimNotifyState_KDealDamage();

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;

	// Make sure you call this when OwnerCharacter is valid
	float CalculateDamage();
	// Make sure you call this when OwnerCharacter is valid (Not doing it locally to save iteration time)
	void DamageActors(TArray<AActor*> &Actors);
	// Make sure you call this when OwnerCharacter is valid (Not doing it locally to save iteration time)
	void SendReaction(AActor* TargetActor);


	// Note CanEditChange is only available when compiling with the editor.
#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

};
