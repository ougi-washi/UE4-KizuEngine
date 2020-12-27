

#include "FunctionLibrary/KActionFunctionLibrary.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

EActionDirection UKActionFunctionLibrary::ConvertFloatToDirection(const float InValue)
{
	if (InValue >= 30 && InValue <= 150) return EActionDirection::AD_Right;
	else if (InValue >= -150 && InValue <= -30) return EActionDirection::AD_Left;
	else if (InValue > -30 && InValue < 30) return EActionDirection::AD_Forward;
	else if (InValue > 150 || InValue < -150) return EActionDirection::AD_Backward;
	return EActionDirection::AD_Backward;
}

EActionDirection UKActionFunctionLibrary::GetCharacterDirection(ACharacter* Character, const TEnumAsByte<EDirectionMode> DirectionMode, float& OutDirectionValue)
{
	if (!Character) {
		UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character Direction."));
		return EActionDirection::AD_Any;
	}
	if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance()) {
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(Character, 0)) {
			FRotator BaseRotation (FRotator::ZeroRotator);
			BaseRotation.Yaw = (DirectionMode == EDirectionMode::CameraRotation) ? CameraManager->GetCameraRotation().Yaw : Character->GetActorRotation().Yaw;
			OutDirectionValue = AnimInstance->CalculateDirection(Character->GetVelocity(), BaseRotation);
			return ConvertFloatToDirection(OutDirectionValue);
		}
		else {
			UE_LOG(LogKizu, Warning, TEXT("Camera Manager not found for this controller, cannot check character Direction."));
			return EActionDirection::AD_Any;
		}
	}
	return EActionDirection::AD_Any;
}

UAnimMontage* UKActionFunctionLibrary::GetRandomMontageFromReactionMontages(TArray<FReactionMontage_Basic> InReactionMontages)
{
	int32 MontageIndex = UKismetMathLibrary::RandomInteger(InReactionMontages.Num());
	if (InReactionMontages.IsValidIndex(MontageIndex))
		return InReactionMontages[MontageIndex].AnimMontage;
	else return nullptr;
}

ERelativePosition UKActionFunctionLibrary::GetRelativePosition(AActor* MainActor, AActor* TargetActor, float& OutOrientationValue)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MainActor->GetActorLocation(), TargetActor->GetActorLocation());
	// Calculate the orientation on the Z axis (Yaw).
	OutOrientationValue = UKismetMathLibrary::NormalizedDeltaRotator(MainActor->GetActorRotation(),LookAtRotation).Yaw;
	// Compare the value of the orientation to know the exact rotation
	if (OutOrientationValue > -135.f && OutOrientationValue < -45.f) return ERelativePosition::RP_Right;
	if (OutOrientationValue > 45.f && OutOrientationValue < 135.f) return ERelativePosition::RP_Left;
	if (OutOrientationValue > -45.f && OutOrientationValue < 45.f) return ERelativePosition::RP_Front;
	if (OutOrientationValue >= 135.f || OutOrientationValue <= -135.f) return ERelativePosition::RP_Back;
	return ERelativePosition::RP_Back;
}

bool UKActionFunctionLibrary::FilterMontagesDataByDirection(ACharacter* Character, TArray<FMontageData> InMontages, TArray<FMontageData>& OutMontages)
{
	if (Character) {
		for (FMontageData MontageData : InMontages) {
			float CharacterDirection = 0;
			if (MontageData.Direction == EActionDirection::AD_Any || MontageData.Direction == GetCharacterDirection(Character, MontageData.DirectionMode, CharacterDirection)) {
				OutMontages.Add(MontageData);
			}
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character Direction and filter out the montages."));
	return false;
}

bool UKActionFunctionLibrary::FilterMontagesDataByState(AKCharacter* KCharacter, TArray<FMontageData> InMontages, TArray<FMontageData>& OutMontages)
{
	if (KCharacter) {
		for (FMontageData MontageData : InMontages) {
			if (MontageData.ValidStates.Contains(KCharacter->ActiveState)) {
				OutMontages.Add(MontageData);
			}
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character state and filter out the montages."));
	return false;
}

bool UKActionFunctionLibrary::FilterReactionsByDirection(ACharacter* Character, AActor* SourceActor, TArray<FReactionMontage_Advanced> InMontages, TArray<FReactionMontage_Advanced>& OutMontages)
{
	if (Character && SourceActor) {
		float Orientation;
		ERelativePosition  RelativePosition = GetRelativePosition(Character, SourceActor, Orientation);
		for (FReactionMontage_Advanced ReactionMontage : InMontages) {
			if (ReactionMontage.DamageSourceRelativePosition == RelativePosition)
				OutMontages.Add(ReactionMontage);
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character or Source Actor Reference, cannot check relative position of the actor and Source Actor."));
	return false;
}

bool UKActionFunctionLibrary::FilterReactionsByState(AKCharacter* KCharacter, TArray<FReactionMontage_Basic> InMontages, TArray<FReactionMontage_Basic>& OutMontages)
{
	if (KCharacter) {
		for (FReactionMontage_Basic ReactionMontage : InMontages)
		{
			if (ReactionMontage.ValidStates.Contains(KCharacter->ActiveState)) 
				OutMontages.Add(ReactionMontage);
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character state and filter out the montages."));
	return false;
}
