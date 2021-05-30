// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "KRespawnTargetPoint.generated.h"

/**
 * 
 */
UCLASS()
class KIZUENGINE_API AKRespawnTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:

	/**
	 * This represents whether or not the respawn point is specific to a player.
	 * If the respawn point is specific to a player, then the ID has to change when being created.
	 * Else, keep it to DEFAULT and it's going to teleport any dead player to here when he's going to respawn.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PlayerID = "DEFAULT";

	// Default is -1 in case there is no specific faction
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Faction = -1;
};
