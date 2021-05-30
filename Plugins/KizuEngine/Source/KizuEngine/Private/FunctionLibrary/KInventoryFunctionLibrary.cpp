// Copyright (c) 2019 Nexus Division. All rights reserved.


#include "FunctionLibrary/KInventoryFunctionLibrary.h"


float UKInventoryFunctionLibrary::GetStatValueFromEquipItem(const FKItemEffect& ItemEffect, const FString StatName /*= "Vitality"*/)
{
	for (FKItemEquipBonus ItemStatBonus : ItemEffect.ItemEquipStatBonuses)
	{
		if (ItemStatBonus.Name == StatName)
		{
			return ItemStatBonus.Value;
		}
	}
	return 0.f;
}

float UKInventoryFunctionLibrary::GetStatValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString StatName /*= "Vitality"*/)
{
	float FinalValue = 0.f;
	for (FKItemEffect ItemEffect : ItemsEffects)
	{
		FinalValue += GetStatValueFromEquipItem(ItemEffect, StatName);
	}
	return FinalValue;
}

float UKInventoryFunctionLibrary::GetResourceValueFromEquipItem(const FKItemEffect& ItemEffect, const FString ResourceName /*= "Stamina"*/)
{
	for (FKItemEquipBonus ItemStatBonus : ItemEffect.ItemEquipResourceBonuses)
	{
		if (ItemStatBonus.Name == ResourceName)
		{
			return ItemStatBonus.Value;
		}
	}
	return 0.f;
}

float UKInventoryFunctionLibrary::GetResourceValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString ResourceName /*= "Stamina"*/)
{
	float FinalValue = 0.f;
	for (FKItemEffect ItemEffect : ItemsEffects)
	{
		FinalValue += GetResourceValueFromEquipItem(ItemEffect, ResourceName);
	}
	return FinalValue;
}

float UKInventoryFunctionLibrary::GetResistanceValueFromEquipItem(const FKItemEffect& ItemEffect, const FString ResistanceName /*= "Fire"*/)
{
	for (FKItemEquipBonus ItemStatBonus : ItemEffect.ItemEquipResistanceBonuses)
	{
		if (ItemStatBonus.Name == ResistanceName)
		{
			return ItemStatBonus.Value;
		}
	}
	return 0.f;
}

float UKInventoryFunctionLibrary::GetResistanceValueFromEquipItems(const TArray<FKItemEffect>& ItemsEffects, const FString ResistanceName /*= "Fire"*/)
{
	float FinalValue = 0.f;
	for (FKItemEffect ItemEffect : ItemsEffects)
	{
		FinalValue += GetResistanceValueFromEquipItem(ItemEffect, ResistanceName);
	}
	return FinalValue;
}
