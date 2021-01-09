// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KizuCombat.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EResourceEffectType
{
	Gain,
	Consumption
};

USTRUCT(BlueprintType)
struct FKResourceSelection
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|General|Resource")
	bool bHealthResource = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|General|Resource")
	FString ResourceName = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|General|Resource")
	float Value = 10.f;
};

USTRUCT(BlueprintType)
struct FSpawnableAbilitySpawnParams
{
	GENERATED_USTRUCT_BODY()

public:

	/** The transform to be spawned with */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability")
		FTransform Transform;
	/** If the SpawnableAbility will be initialized. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability")
		uint8 bInitizalizeMobility : 1;
	/** The initial Direction of the Spawnable */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability", meta = (EditCondition = "bInitizalizeMobility"))
		FVector InitialDirection = FVector::ZeroVector;
	/** A target actor  homing. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability", meta = (EditCondition = "bInitizalizeMobility"))
		AActor* TargetActor;

	FSpawnableAbilitySpawnParams() {
		bInitizalizeMobility = true;
	}
};


USTRUCT(BlueprintType)
struct FTimeDilationParams
{
	GENERATED_USTRUCT_BODY()

public:

	/** To have a natural state, the value has to be 1. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Kizu|General")
		float Value = .7f;
};

UCLASS()
class KIZUENGINE_API UKizuCombat : public UObject
{
	GENERATED_BODY()
	
};
