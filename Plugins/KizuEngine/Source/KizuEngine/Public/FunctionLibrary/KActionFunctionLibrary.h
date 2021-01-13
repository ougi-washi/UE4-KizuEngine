

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/KAction.h"
#include "KActionFunctionLibrary.generated.h"

/**
 * 
 */

class UDataTable;

UCLASS()
class KIZUENGINE_API UKActionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static EActionDirection ConvertFloatToDirection(const float InValue);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static EActionDirection GetCharacterDirection(ACharacter* Character, const TEnumAsByte<EDirectionMode> DirectionMode, float &OutDirectionValue);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static UAnimMontage* GetRandomMontageFromReactionMontages(TArray<FReactionMontage_Basic> InReactionMontages);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static ERelativePosition GetRelativePosition(AActor* MainActor, AActor* TargetActor, float& OutOrientationValue);
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
	
	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static bool FilterMontagesDataByState(AKCharacter* KCharacter, TArray<FMontageData> InMontages, TArray<FMontageData>& OutMontages);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static bool FilterReactionsByDirection(ACharacter* Character, AActor* SourceActor, TArray<FReactionMontage_Advanced> InMontages, TArray<FReactionMontage_Advanced>& OutMontages);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static bool FilterReactionsByState(AKCharacter* KCharacter, TArray<FReactionMontage_Basic> InMontages, TArray<FReactionMontage_Basic>& OutMontages);

	/**
	 * Data Tables section
	 */

	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	static bool ExecuteActionFromDT(AKCharacter* KCharacter, UDataTable* ActionDataTable, FName ActionRowName, bool bUseCooldown = true);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Reaction")
	static bool SendReaction(AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter, const FReactionSendingData& ReactionSendingData);

};
