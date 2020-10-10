// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/KCharacter.h"
#include "KCombatCharacter.generated.h"

struct FAbilityData;
class UDataTable;

/**
 * 
 */
UCLASS()
class KIZUENGINE_API AKCombatCharacter : public AKCharacter
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Kizu|Combat|Character")
	bool ExecuteAbility(const FAbilityData &AbilityData);
	UFUNCTION(BlueprintCallable, Category = "Kizu|Combat|Character")
	bool ExecuteAbilityFromDT(const UDataTable *DataTable, const FString AbilityID);
};
