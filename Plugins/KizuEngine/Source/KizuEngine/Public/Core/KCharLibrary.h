// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "Engine/Engine.h"
#include "KCharLibrary.generated.h"

USTRUCT(BlueprintType)
struct FKResource {

	GENERATED_USTRUCT_BODY()

public:
	/** The custom resource name (ID)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "None";

	/** The custom resource max value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxValue = 100.f;

	/** The custom resource current value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentValue = 100.f;

	/** Whether the value can be below 0 or not. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	bool bCanBeBelowZero = false;

	/** Display Name for UI if needed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Visual)
	FText DisplayName;
};


USTRUCT(BlueprintType)
struct FKStat {

	GENERATED_BODY()

public:

	/** The custom Stat name (ID)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = General)
	FString Name = "None";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = General)
	float CurrentValue = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = General)
	float BaseValue = 0.f;

	/** Display Name for UI if needed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Visual)
	FText DisplayName;
};


USTRUCT(BlueprintType)
struct FKResourceRegeneration {

	GENERATED_USTRUCT_BODY()

public:

	/** The custom resource name (DEFAULT_HEALTH for health) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "None";
	/** The value to regenerate every second */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float RegenValue = 1.f;
	/** How much does it take to trigger the regeneration (default = every 1 second) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float TickingRate = 1.f;
	/** Timer handle to manage this resource regeneration ticking */
	FTimerHandle TimerHandle;
};

USTRUCT(BlueprintType)
struct FKCharacterData
{
	GENERATED_USTRUCT_BODY()

public:
	/** The character name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "Character_Name";

	/** The character max health. The current health can never exceed this amount */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxHealth = 100.f;

	/** The character current health. When reduced to 0, they are considered dead. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentHealth = 100.f;

	/** Custom resources array (Examples: Energy, Mana, Armors..) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResource> Resources;

	/** Resource Regeneration array (Name has to be available in Resources) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResourceRegeneration> ResourcesRegen;

	/** Custom Stat array (Examples: Strength, Vitality..etc) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKStat> Stats;

	/** Faction to define either it's an enemy or an ally to another Faction */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	uint8 Faction = 0;

	/** */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	int32 Level = 0;
};

USTRUCT(BlueprintType)
struct FCustomDamage
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	FString ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	float Value;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	TSubclassOf<UDamageType> DamageType;

	FORCEINLINE bool operator==(const FCustomDamage& rhs) const
	{
		return (rhs.ID == this->ID);
	}

	FCustomDamage() {
		// Default Constructor
	}

	FCustomDamage(FString InID) {
		this->ID = InID;
	}
};