// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KIZUENGINE_API AKPlayerController : public APlayerController
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Kizu")
		bool CreateSession(int32 playerCount);
};
