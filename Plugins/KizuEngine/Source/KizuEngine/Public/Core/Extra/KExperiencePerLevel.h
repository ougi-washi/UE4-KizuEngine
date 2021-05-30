// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KExperiencePerLevel.generated.h"

/**
 * 
 */
UCLASS()
class KIZUENGINE_API UKExperiencePerLevel : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TArray<int32> ExperiencePerLevel = {
		100, 300, 500, 700, 900, 1000, 1200, 1400, 1600, 1700,
		1900, 2100, 2300, 2500, 2700, 2900, 3100, 3400, 3600, 3800
	};

	int32 GetExperience(const int32 Level);
};
