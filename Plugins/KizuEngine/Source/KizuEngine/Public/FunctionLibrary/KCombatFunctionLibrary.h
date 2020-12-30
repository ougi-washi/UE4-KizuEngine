// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/Combat/KizuCombat.h"
#include "KCombatFunctionLibrary.generated.h"
/**
 * 
 */

class AKBuff;
class AKSpawnableAbility;


UCLASS()
class KIZUENGINE_API UKCombatFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Spawns a buff (Not replicated)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff")
	static AKBuff* SpawnBuff(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform);
	/**
	 * Spawns a Spawnable ability (Not replicated)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff")
	static AKSpawnableAbility* SpawnSpawnableAbility(AActor* OwnerActor, TSubclassOf<AKSpawnableAbility> SpawnableAbilityToSpawn, FSpawnableAbilitySpawnParams SpawnParams);
};
