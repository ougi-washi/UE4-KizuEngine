

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/KAction.h"
#include "KActionFunctionLibrary.generated.h"

/**
 * 
 */


UCLASS()
class KIZUENGINE_API UKActionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static bool IsValidStateForCharacterByMontage(const FMontageData& MontageData, AKCharacter* KCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static bool IsValidStateForCharacterByAction(const FActionData& ActionData, AKCharacter* KCharacter, const bool bUseComboCounter = true);


	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static EActionDirection ConvertFloatToDirection(const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static EActionDirection GetCharacterDirection(ACharacter* Character, const TEnumAsByte<EDirectionMode> DirectionMode, float &OutDirectionValue);
	/**
	 * Filters our montages in the action by picking only the ones being in the same direction or having "Any" as direction with the Character given into the argument.
	 * Returns false if the process fails due to an invalid reference or empty array. 
	 * @param Character - The character used to define the direction.
	 * @param InActionData - The action data to filter out.
	 * @param OutMontages - The resulting montages found in the action.
	 * @return Either it was a success or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static bool FilterMontagesDataByDirection(ACharacter* Character, TArray<FMontageData> InMontages, TArray<FMontageData> &OutMontages);
	
	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static bool FilterMontagesDataByState(AKCharacter* KCharacter, TArray<FMontageData> InMontages, TArray<FMontageData>& OutMontages);
};
