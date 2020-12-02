// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "Core/Inventory/KItem.h"
#include "KInventory.generated.h"

USTRUCT(BlueprintType)
struct FInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FItem> Items;
};

/**
 * 
 */
UCLASS()
class KIZUENGINE_API UKInventory : public UObject
{
	GENERATED_BODY()
public:
	UKInventory();
	~UKInventory();
};
