

#include "FunctionLibrary/KActionFunctionLibrary.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UKActionFunctionLibrary::IsValidStateForCharacterByMontage(const FMontageData& MontageData, AKCharacter* KCharacter)
{
	if (KCharacter && MontageData.ValidStates.Num() > 0) {
		return MontageData.ValidStates.Contains(KCharacter->ActiveState);
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference or the valid states are null for this Montage Data, cannot check if there is a valid state here."));
	return false;
}

bool UKActionFunctionLibrary::IsValidStateForCharacterByAction(const FActionData& ActionData, AKCharacter* KCharacter, const bool bUseComboCounter)
{
	if (!KCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check Action Data."));
		return false;
	}

	if (bUseComboCounter)
	{
		if (ActionData.MontagesData.IsValidIndex(KCharacter->ComboCounter))
			return IsValidStateForCharacterByMontage(ActionData.MontagesData[KCharacter->ComboCounter], KCharacter);
	}

	for (FMontageData MontageData : ActionData.MontagesData)
	{
		if (!IsValidStateForCharacterByMontage(MontageData, KCharacter)) 
			return false;
	}
	return true;
}

EActionDirection UKActionFunctionLibrary::ConvertFloatToDirection(const float InValue)
{
	if (InValue >= 30 && InValue <= 150) return EActionDirection::AD_Right;
	else if (InValue >= -150 && InValue <= -30) return EActionDirection::AD_Left;
	else if (InValue > -30 && InValue < 30) return EActionDirection::AD_Forward;
	else if (InValue > 150 && InValue < -150) return EActionDirection::AD_Backward;
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
			if (IsValidStateForCharacterByMontage(MontageData, KCharacter)) {
				OutMontages.Add(MontageData);
			}
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character Direction and filter out the montages."));
	return false;
}
