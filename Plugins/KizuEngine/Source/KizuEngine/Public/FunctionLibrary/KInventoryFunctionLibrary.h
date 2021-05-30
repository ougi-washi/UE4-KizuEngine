// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/Inventory/KItem.h"
#include "KInventoryFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class KIZUENGINE_API UKInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetStatValueFromEquipItem(const FKItemEffect& ItemEffect, const FString StatName = "Vitality");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetStatValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString StatName = "Vitality");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetResourceValueFromEquipItem(const FKItemEffect& ItemEffect, const FString ResourceName = "Stamina");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetResourceValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString ResourceName = "Stamina");
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetResistanceValueFromEquipItem(const FKItemEffect& ItemEffect, const FString ResistanceName = "Fire");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Inventory")
	static float GetResistanceValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString ResistanceName = "Fire");

};
