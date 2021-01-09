// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "Core/Inventory/KItem.h"
#include "KInventory.generated.h"

USTRUCT(BlueprintType)
struct FKInventoryData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FKItemData> Items;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	uint8 ConsiderMaxSlots : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxSlots = 100;

	int32 FindItem(const FKItemData& Item) {
		return Items.Find(Item);
		
	}
	int32 GetItemCount(const FKItemData& Item) {
		int32 ItemCount = 0;
		for (FKItemData TempItem : Items) {
			if (TempItem == Item)
				ItemCount++;
		}
		return ItemCount;
	}
	/** Adds item to the Items stack multiple times depending on the Amount*/
	void AddItem(const FKItemData& Item, const uint32 Amount) {
		// Checks if MaxSlots are going to be taken into consideration or ignored
		if (!ConsiderMaxSlots || (ConsiderMaxSlots && !Items.Contains(Item) && CalculateFreeSlots() > MaxSlots))
			for (uint32 i = 0 ; i < Amount ; i++) Items.Add(Item);
	}
	/** Adds one single item to the Items stack */
	void AddItem(const FKItemData& Item) {
		AddItem(Item, 1);
	}
	void RemoveItem(const FKItemData& Item, const uint32 Amount) {
		for (uint32 i = 0; i < Amount; i++) {
			int32 FoundIndex = Items.Find(Item);
			if (FoundIndex != INDEX_NONE)
				Items.RemoveAt(FoundIndex);
		}
	}
	void RemoveItem(const FKItemData& Item) {
		RemoveItem(Item, 1);
	}
	int CalculateUsedSlots() {
		TArray<FKItemData> TempItemArray;
		for (FKItemData Item : Items)
			TempItemArray.AddUnique(Item);
		return TempItemArray.Num();
	}
	int CalculateFreeSlots() {
		return (MaxSlots - CalculateUsedSlots());
	}
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
