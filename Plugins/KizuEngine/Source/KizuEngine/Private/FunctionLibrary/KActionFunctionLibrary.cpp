

#include "FunctionLibrary/KActionFunctionLibrary.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UKActionFunctionLibrary::IsValidStateForCharacterByMontage(const FMontageData& MontageData, AKCharacter* KCharacter)
{
	if (KCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("ActiveState : [%s] Displaying Valid states now : "), *KCharacter->ActiveState);
		for (FString ValidState : MontageData.ValidStates) {
			UE_LOG(LogKizu, Warning, TEXT("ActiveState : %s"), *ValidState);
		}
		return MontageData.ValidStates.Contains(KCharacter->ActiveState);
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check Montage Data."));
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
	if (InValue > -30 && InValue < 30) return EActionDirection::AD_Forward;
	if (InValue > 150 && InValue < -150) return EActionDirection::AD_Backward;
	if (InValue >= 30 && InValue <= 150) return EActionDirection::AD_Right;
	if (InValue >= -150 && InValue <= -30) return EActionDirection::AD_Left;
	return EActionDirection::AD_Any;
}

EActionDirection UKActionFunctionLibrary::GetCharacterDirection(ACharacter* Character, const TEnumAsByte<EDirectionMode> DirectionMode)
{
	if (!Character) {
		UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character Direction."));
		return EActionDirection::AD_Any;
	}
	if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance()) {
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(Character, 0)) {
			FRotator BaseRotation (FRotator::ZeroRotator);
			BaseRotation.Yaw = (DirectionMode == EDirectionMode::CameraRotation) ? CameraManager->GetCameraRotation().Yaw : Character->GetActorRotation().Yaw;
			return ConvertFloatToDirection(AnimInstance->CalculateDirection(Character->GetVelocity(), BaseRotation));
		}
		else {
			UE_LOG(LogKizu, Warning, TEXT("Camera Manager not found for this controller, cannot check character Direction."));
			return EActionDirection::AD_Any;
		}
	}
	return EActionDirection::AD_Any;
}

bool UKActionFunctionLibrary::GetMontagesDataByDirection(ACharacter* Character, const FActionData& InActionData, TArray<FMontageData>& OutMontages)
{
	if (Character) {
		for (FMontageData MontageData : InActionData.MontagesData) {
			if (MontageData.Direction == EActionDirection::AD_Any || MontageData.Direction == GetCharacterDirection(Character, MontageData.DirectionMode)) {
				OutMontages.Add(MontageData);
			}
		}
		if (OutMontages.Num() > 0)
			return true;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Invalid Character Reference, cannot check character Direction and filter out the montages."));
	return false;
}
