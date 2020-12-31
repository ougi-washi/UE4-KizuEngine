// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_KSpawnSpawnableAbility.generated.h"

class AKSpawnableAbility;

/**
 * 
 */

UCLASS(Blueprintable, meta = (DisplayName = "Spawn SpawnableAbility (Kizu)"))
class KIZUENGINE_API UAnimNotify_KSpawnSpawnableAbility : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Kizu")
	TSubclassOf<AKSpawnableAbility> SpawnableAbilityToSpawn;

	UPROPERTY(EditAnywhere, Category = "Kizu")
	uint8 bInitializeMovement : 1;

	/** What is the socket name where the Ability is going to be spawned at in the Mesh */
	UPROPERTY(EditAnywhere, Category = "Kizu")
	FName SocketName = "None";

	UPROPERTY(EditAnywhere, Category = "Kizu")
	uint8 bUseCrosshair : 1;

	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "bUseCrosshair"))
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, Category = "Kizu")
	uint8 bUseResourceAsRange : 1;

	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "!bUseResourceAsRange"))
	float TargettingRange = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "bUseResourceAsRange"))
	FString ResourceName = "None";

	UPROPERTY(EditAnywhere, Category = "Kizu", meta = (EditCondition = "bUseResourceAsRange"))
	float ResourcePercentage = 100.f;

	//Constructor
	UAnimNotify_KSpawnSpawnableAbility();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
