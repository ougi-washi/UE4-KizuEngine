// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "AnimNotify/AnimNotifyState_KMelee.h"
#include "Core/KCharacter.h"
#include "FunctionLibrary/KCombatFunctionLibrary.h"
#include "Engine/DataTable.h"


UAnimNotifyState_KMelee::UAnimNotifyState_KMelee() : Super()
{
	bIgnoreSelf = true;
	bAffectOtherFaction = true;
	bUseCharacterReactionDataTable = true;
}

void UAnimNotifyState_KMelee::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	OwnerCharacter = Cast<AKCharacter>(MeshComp->GetOwner());
	if (OwnerCharacter) {
		CollisionComponents = OwnerCharacter->GetComponentsByTag(UPrimitiveComponent::StaticClass(), CollisionComponentTag);
		FinalDamage = CalculateDamage();
	}
	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_KMelee::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsValid(OwnerCharacter))
		if (OwnerCharacter->IsLocallyControlled())
			for (UActorComponent* CollisionComponent : CollisionComponents) {
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(CollisionComponent)) {
					TArray<AActor*> OverlappedActors;
					PrimComp->GetOverlappingActors(OverlappedActors);
					AffectActors(OverlappedActors);
				}
			}
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_KMelee::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	DamagedActors.Empty();
	Received_NotifyEnd(MeshComp, Animation);
}

float UAnimNotifyState_KMelee::CalculateDamage()
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

void UAnimNotifyState_KMelee::AffectActors(TArray<AActor*>& Actors)
{
	for (AActor* TempActor : Actors) {
		bool bSameFaction = UKCombatFunctionLibrary::IsSameFactionWithCast(OwnerCharacter, TempActor);
		if (!bIgnoreSelf && TempActor == OwnerCharacter) 
			AffectActor(TempActor);
		if (bAffectOtherFaction && !bSameFaction)
			AffectActor(TempActor);
		if (bAffectSelfFaction && bSameFaction && TempActor != OwnerCharacter)
			AffectActor(TempActor);
	}
}

void UAnimNotifyState_KMelee::AffectActor(AActor* Actor)
{
	if (Actor) {
		if (!DamagedActors.Contains(Actor)) {
			DamagedActors.AddUnique(Actor);
			OwnerCharacter->ApplyDamage_Replicated(Actor, FinalDamage, DamageType);
			if (bSendReaction) SendReaction(Actor);
		}
	}
}

void UAnimNotifyState_KMelee::SendReaction(AActor* TargetActor)
{
	if (AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor)) {
		static const FString ContextString(TEXT("ReactionDataTable"));

		FReactionData* ReactionData = nullptr;
		if (bUseCharacterReactionDataTable && OwnerCharacter->ReactionDataTable)
			ReactionData = OwnerCharacter->ReactionDataTable->FindRow<FReactionData>(FName(ReactionRowName), ContextString, true);
		else if (ReactionDataTable)
			ReactionData = ReactionDataTable->FindRow<FReactionData>(FName(ReactionRowName), ContextString, true);

		if (ReactionData) {
			if (bSmoothReaction) {
				OwnerCharacter->SendReaction_Replicated(*ReactionData, TargetCharacter, RR_SkipSource); // Execute the reaction on everybody else besides the source (takes time as it depends on the bandwidth)
				OwnerCharacter->SendReaction_Replicated(*ReactionData, TargetCharacter, RR_SourceOnly); // Execute the reaction for the source
			}
			else {
				OwnerCharacter->SendReaction_Replicated(*ReactionData, TargetCharacter, RR_All);
			}
		}	
		else UE_LOG(LogTemp, Warning, TEXT("Unable to find row in order to send a reaction."))
	}
}

#if WITH_EDITOR
bool UAnimNotifyState_KMelee::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);
	// Can edit Damage Value (Static)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KMelee, DamageValue))
	{
		return ValueSource == EValueSource::Static;
	}
	// Can edit Resource Name (Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KMelee, ResourceName))
	{
		return ValueSource == EValueSource::Dynamic;
	}
	// Can edit Damage Percentage (Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KMelee, PercentageValue))
	{
		return ValueSource == EValueSource::Dynamic;
	}
	// Can edit DamageType (Static and Dynamic)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KMelee, DamageType))
	{
		return (ValueSource != EValueSource::Custom);
	}
	// Can edit Damage Percentage (Custom)
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_KMelee, CustomDamageID))
	{
		return ValueSource == EValueSource::Custom;
	}
	return ParentVal;
}
#endif