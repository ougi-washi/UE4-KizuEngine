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
struct FResourceSelection
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
