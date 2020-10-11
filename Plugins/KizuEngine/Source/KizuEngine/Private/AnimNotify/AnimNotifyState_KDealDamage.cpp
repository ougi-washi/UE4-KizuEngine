// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "AnimNotify/AnimNotifyState_KDealDamage.h"
#include "Core/KCharacter.h"

void UAnimNotifyState_KDealDamage::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	OwnerCharacter = Cast<AKCharacter>(MeshComp->GetOwner());
	if (OwnerCharacter) {
		CollisionComponents = OwnerCharacter->GetComponentsByTag(UPrimitiveComponent::StaticClass(), CollisionComponentTag);
		FinalDamage = CalculateDamage();
	}
	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_KDealDamage::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (OwnerCharacter)
		for (UActorComponent* CollisionComponent : CollisionComponents) {
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(CollisionComponent)) {
				TArray<AActor*> OverlappedActors;
				PrimComp->GetOverlappingActors(OverlappedActors);
				DamageActors(OverlappedActors);
			}
		}
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_KDealDamage::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	DamagedActors.Empty();
	Received_NotifyEnd(MeshComp, Animation);
}

float UAnimNotifyState_KDealDamage::CalculateDamage()
{
	if (ValueSource == EValueSource::Static) return DamageValue;
	else if (ValueSource == EValueSource::Dynamic) {
		float CurrentResourceValue;
		if (OwnerCharacter->GetResourceCurrentValue(ResourceName, CurrentResourceValue)) {
			return (CurrentResourceValue * PercentageValue / 100);
		}
	}
	else if (ValueSource == EValueSource::Custom) {
		FCustomDamage CustomDamage;
		if (OwnerCharacter->GetCustomDamage(CustomDamageID, CustomDamage))
			return CustomDamage.Value;
	}
	return 0;
}

void UAnimNotifyState_KDealDamage::DamageActors(TArray<AActor*>& Actors)
{
	for (AActor* ActorToDamage : Actors) {
		if (!DamagedActors.Contains(ActorToDamage)) {
			OwnerCharacter->ServerApplyDamage(ActorToDamage, FinalDamage, DamageType);
			DamagedActors.AddUnique(ActorToDamage);
		}
	}
}

bool UAnimNotifyState_KDealDamage::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);
	// Can edit Damage Value (Static)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KDealDamage, DamageValue))
	{
		return ValueSource == EValueSource::Static;
	}
	// Can edit Resource Name (Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KDealDamage, ResourceName))
	{
		return ValueSource == EValueSource::Dynamic;
	}
	// Can edit Damage Percentage (Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KDealDamage, PercentageValue))
	{
		return ValueSource == EValueSource::Dynamic;
	}
	// Can edit DamageType (Static and Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KDealDamage, DamageType))
	{
		return (ValueSource != EValueSource::Custom);
	}
	// Can edit Damage Percentage (Custom)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KDealDamage, CustomDamageID))
	{
		return ValueSource == EValueSource::Custom;
	}
	return ParentVal;
}
