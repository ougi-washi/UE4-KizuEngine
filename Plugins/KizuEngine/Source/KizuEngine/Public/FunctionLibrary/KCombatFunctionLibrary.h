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
class AKCharacter;
class AKRespawnTargetPoint;
class UDamageType;

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
	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability")
	static AKSpawnableAbility* SpawnSpawnableAbility(AActor* OwnerActor, TSubclassOf<AKSpawnableAbility> SpawnableAbilityToSpawn, FSpawnableAbilitySpawnParams SpawnParams);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff")
	static bool IsSameFaction(AKCharacter* CharacterOne, AKCharacter* CharacterTwo);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff")
	static bool IsSameFactionWithCast(AKCharacter* CharacterOne, AActor* CharacterTwo);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Respawn")
	static AKRespawnTargetPoint* GetClosestRespawnTargetPoint(AKCharacter* Character, const FString PlayerID = "DEFAULT");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Respawn")
	static bool GetClosestRespawnTargetPointLocRot(FVector &OutLocation, FRotator &OutRotation, AKCharacter* Character, const FString PlayerID = FString("DEFAULT"), const FVector LocationOffset = FVector(0.f, 0.f, 80.f));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Respawn")
	static TSubclassOf<UDamageType> GetDamageType(UPARAM(ref)FDamageEvent &DamageEvent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Damage")
	static void GetVariance(float& OutMin, float& OutMax, const float InValue = 200.f, const float InVariancePercentage = 5.f);
};
