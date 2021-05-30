// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Combat/KSpawnableAbility.h"
#include "KProjectileAbilitySpawner.generated.h"

/**
 * 
 */
UCLASS()
class KIZUENGINE_API AKProjectileAbilitySpawner : public AKSpawnableAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data")
	TSubclassOf<AKSpawnableAbility> AbilityToSpawn = nullptr;

	virtual void OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
};
