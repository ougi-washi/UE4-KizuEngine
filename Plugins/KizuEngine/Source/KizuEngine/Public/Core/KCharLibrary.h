// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "Engine/Engine.h"
#include "KCharLibrary.generated.h"


USTRUCT(BlueprintType)
struct FKResourceModifier {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		FString StatName = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		float ModifierValue = 3.f;

	FKResourceModifier()
	{
		// Default Constructor
	}

	FKResourceModifier(const FString NewStatName, const float NewModifierValue)
	{
		StatName = NewStatName;
		ModifierValue = NewModifierValue;
	}
};

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

	/** Whether there is a max value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	bool bCanBeCapped = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResourceModifier> ResourceModifiers = {
		FKResourceModifier("Magic", 3.f),
		FKResourceModifier("Intelligence", 3.f)
	};

	/** Display Name for UI if needed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Visual)
	FText DisplayName;

	FKResource()
	{
		//Default Constructor
	};

	FKResource(	const FString NewName, 
				const TArray<FKResourceModifier> NewResourceModifiers,
				const FString NewDisplayName = "RESOURCE_NAME",
				const float NewMaxValue = 100.f, 
				const float NewCurrentValue = 100.f,
				const bool bNewCanBeBelowZero = false, 
				const bool bNewCanBeCapped = true)
	{
		Name = NewName;
		ResourceModifiers = NewResourceModifiers;
		DisplayName = FText::FromString(NewDisplayName);
		MaxValue = NewMaxValue;
		CurrentValue = NewCurrentValue;
		bCanBeBelowZero = bNewCanBeBelowZero;
		bCanBeCapped = bNewCanBeCapped;
	};
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
	FText DisplayName = FText::FromString("Stat_Name");

	FKStat()
	{
		//Default Constructor
	};
	
	FKStat(const FString NewName, const float NewCurrentValue, const float NewBaseValue, const FText NewDisplayName) 
	{
		Name = NewName;
		CurrentValue = NewCurrentValue;
		BaseValue = NewBaseValue;
		DisplayName = NewDisplayName;
	};
};

USTRUCT(BlueprintType)
struct FKStatLevelPoint {

	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString StatName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	int32 LevelPoints;

	FKStatLevelPoint()
	{
		// Default Constructor
		StatName = "None";
		LevelPoints = 0;
	};

	FKStatLevelPoint(const FString NewStatName, const int32 NewLevelPoints = 0)
	{
		StatName = NewStatName;
		LevelPoints = NewLevelPoints;
	};
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

	FKResourceRegeneration()
	{
		// Default Constructor
	};

	FKResourceRegeneration(const FString NewName, const float NewRegenValue = 2.f, const float NewTickingRate = 1.f)
	{
		Name = NewName;
		RegenValue = NewRegenValue;
		TickingRate = NewTickingRate;
	};
};


USTRUCT(BlueprintType)
struct FKResistance {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float Value;

	FKResistance()
	{
		// Default Constructor
	};

	FKResistance(const FString NewName, const float NewValue)
	{
		Name = NewName;
		Value = NewValue;
	};
};

USTRUCT(BlueprintType)
struct FKElementalDamage {

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		float Value;

	FKElementalDamage()
	{
		// Default Constructor
	};

	FKElementalDamage(const FString NewName, const float NewValue)
	{
		Name = NewName;
		Value = NewValue;
	};
};


USTRUCT(BlueprintType)
struct FKExperienceKillData{

	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanGainExperience = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ExperienceOnKill = 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ExperienceReductionByLevelDiff = 10;

	int32 GetExperienceOnKill(const int32 KillerLevel = 5, const int KilledLevel = 5)
	{
		if (KilledLevel >= KillerLevel)
		{
			return ExperienceOnKill;
		}
		int32 ResultExperience = (ExperienceOnKill - (ExperienceReductionByLevelDiff * (KillerLevel - KilledLevel)));
		return (ResultExperience > 0) ? ResultExperience : 0;
	}
};

USTRUCT(BlueprintType)
struct FKCharacterData
{
	GENERATED_USTRUCT_BODY()

public:

	/** The character name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "Character_Name";

	/** General Progression Level */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	int32 ProgressionLevel = 1;

	/** General Progression Experience */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	int32 ProgressionExperience = 0;

	/** The character max health. The current health can never exceed this amount */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxHealth = 100.f;

	/** The character current health. When reduced to 0, they are considered dead. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentHealth = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResourceModifier> HealthResourceModifiers = {
		FKResourceModifier("Vitality", 3.f),
	};

	/** Custom resources array (Examples: Energy, Mana, Armors..) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResource> Resources;

	/** Resource Regeneration array (Name has to be available in Resources) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResourceRegeneration> ResourcesRegen = 
	{
		FKResourceRegeneration("Stamina", 5.f),
		FKResourceRegeneration("Mana", 5.f)
	};

	/** Custom Stat array (Examples: Strength, Vitality..etc) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKStat> Stats;
	
	/** The points that will be placed/set in to the StatLevelPoints */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	int32 UnspentStatPoints = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKStatLevelPoint> StatLevelPoints;

	/** Resistances array */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResistance> Resistances;

	/** Elemental Damages bonus array */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKElementalDamage> ElementalDamages;

	/** Faction to define either it's an enemy or an ally to another Faction */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	uint8 Faction = 0;


	FKCharacterData()
	{
		// Default Constructor
		InitData();
	};

	bool GetStat(const FString StatName, FKStat& OutStat)
	{
		for (FKStat &Stat : Stats)
		{
			if (Stat.Name == StatName)
			{
				OutStat = Stat;
				return true;
			}
		}
		return false;
	}

	bool GetResource(const FString ResourceName, FKResource& OutResource)
	{
		for (FKResource& Resource : Resources)
		{
			if (Resource.Name == ResourceName)
			{
				OutResource = Resource;
				return true;
			}
		}
		return false;
	}

	// Returns the total resource value by accessing the stats and calculating from the resources modifiers. If not a single stat found, returns the default value.
	float CalculateResourceValue(const TArray<FKResourceModifier> InResourceModifiers, const float DefaultValue)
	{
		float TotalValue = 0.f;
		bool bFoundStats = false;
		for (FKResourceModifier ResourceModifier : InResourceModifiers)
		{
			FKStat OutStat;
			if (GetStat(ResourceModifier.StatName, OutStat))
			{
				bFoundStats = true;
				TotalValue += (OutStat.CurrentValue * ResourceModifier.ModifierValue);
			}
		}
		if (bFoundStats)
		{
			return TotalValue;
		}
		return DefaultValue;
	}

	void AddStatLevelPoints(const FString StatName, const int32 StatPoints = 1)
	{
		for (FKStatLevelPoint &StatLevelPoint : StatLevelPoints)
		{
			if (StatLevelPoint.StatName == StatName)
			{
				StatLevelPoint.LevelPoints += StatPoints;
			}
		}
	}

	// Update a stat from stat level points
	void UpdateStatFromLevels()
	{
		for (FKStat& Stat : Stats)
		{
			for (FKStatLevelPoint& StatLevelPoint : StatLevelPoints)
			{
				if (StatLevelPoint.StatName == Stat.Name)
				{
					Stat.BaseValue += StatLevelPoint.LevelPoints;
					Stat.CurrentValue += StatLevelPoint.LevelPoints;
				}
			}
		}
	}

	void UpdateResource(FKResource &ResourceToUpdate)
	{
		float NewValue = CalculateResourceValue(ResourceToUpdate.ResourceModifiers, 1000.f);
		ResourceToUpdate.MaxValue = NewValue;
		ResourceToUpdate.CurrentValue = NewValue;
	}

	void UpdateAllResources()
	{
		// Health
		MaxHealth = CalculateResourceValue(HealthResourceModifiers, 1000.f);
		CurrentHealth = MaxHealth;

		// All other custom resources
		for (FKResource &Resource : Resources)
		{
			UpdateResource(Resource);
		}
	}

	void InitResources()
	{
		// Health
		MaxHealth = CalculateResourceValue(HealthResourceModifiers, 1000.f);
		CurrentHealth = MaxHealth;

		// Mana
		TArray<FKResourceModifier> ManaModifiers =	{	FKResourceModifier("Magic", 5.f), 
														FKResourceModifier("Intelligence", 1.5f) };
		float ManaValue = CalculateResourceValue(ManaModifiers, 100.f);
		FKResource ManaResource("Mana", ManaModifiers,"Mana", ManaValue, ManaValue);

		// Stamina
		TArray<FKResourceModifier> StaminaModifiers = {	FKResourceModifier("Dexterity", 2.f),
														FKResourceModifier("Strength", 1.5f),
														FKResourceModifier("Vitality", 3.f) };
		float StaminaValue = CalculateResourceValue(StaminaModifiers, 100.f);
		FKResource StaminaResource = FKResource("Stamina", StaminaModifiers, "Stamina", StaminaValue, StaminaValue);

		// Physical Damage
		TArray<FKResourceModifier> PhysicalDamageModifiers = {	FKResourceModifier("Strength", 1.5f),
																FKResourceModifier("Dexterity", 3.f) };
		float PhysicalDamageValue = CalculateResourceValue(PhysicalDamageModifiers, 100.f);
		FKResource PhysicalDamageResource = FKResource("PhysicalDamage", PhysicalDamageModifiers, "Physical Damage", PhysicalDamageValue, PhysicalDamageValue);

		// Magical Damage
		TArray<FKResourceModifier> MagicalDamageModifiers = {	FKResourceModifier("Magic", 1.5f),
																FKResourceModifier("Intelligence", 3.f)};
		float MagicalDamageValue = CalculateResourceValue(MagicalDamageModifiers, 100.f);
		FKResource MagicalDamageResource = FKResource("MagicalDamage", MagicalDamageModifiers, "Magical Damage", MagicalDamageValue, MagicalDamageValue);

		// Physical Defense
		TArray<FKResourceModifier> PhysicalDefenseModifiers = { FKResourceModifier("Vitality", 2.f),
																FKResourceModifier("Strength", 2.f)};
		float PhysicalDefenseValue = CalculateResourceValue(MagicalDamageModifiers, 100.f);
		FKResource PhysicalDefenseResource = FKResource("PhysicalDefense", PhysicalDefenseModifiers, "Physical Defense", PhysicalDefenseValue, PhysicalDefenseValue);
		
		// Magical Defense
		TArray<FKResourceModifier> MagicalDefenseModifiers = { FKResourceModifier("Vitality", 2.f),
																FKResourceModifier("Magic", 2.f) };
		float MagicalDefenseValue = CalculateResourceValue(MagicalDamageModifiers, 100.f);
		FKResource MagicalDefenseResource = FKResource("MagicalDefense", MagicalDefenseModifiers, "Magical Defense", MagicalDefenseValue, MagicalDefenseValue);

		// Dodge Chance
		TArray<FKResourceModifier> DodgeChanceModifiers = { FKResourceModifier("Luck", 0.1f) };
		float DodgeChanceValue = CalculateResourceValue(DodgeChanceModifiers, 2.f);
		FKResource DodgeChanceResource = FKResource("DodgeChance", DodgeChanceModifiers, "Dodge Chance", DodgeChanceValue, DodgeChanceValue, false, false);

		// Critical Chance
		TArray<FKResourceModifier> CriticalChanceModifiers = { FKResourceModifier("Luck", 0.3f) };
		float CriticalChanceValue = CalculateResourceValue(CriticalChanceModifiers, 2.f);
		FKResource CriticalChanceResource = FKResource("CriticalChance", CriticalChanceModifiers, "Critical Chance", CriticalChanceValue, CriticalChanceValue, false, false);

		// Critical Damage Bonus
		TArray<FKResourceModifier> CriticalDamageModifiers = { FKResourceModifier("Luck", 0.2f) };
		float CriticalDamageValue = CalculateResourceValue(CriticalDamageModifiers, 2.f);
		FKResource CriticalDamageResource = FKResource("CriticalDamage", CriticalDamageModifiers, "Critical Damage Bonus", CriticalDamageValue, CriticalDamageValue, false, false);

		// Casting Chance
		TArray<FKResourceModifier> CastChanceModifiers = { FKResourceModifier("Luck", 5.f) };
		float CastChanceValue = CalculateResourceValue(CastChanceModifiers, 40.f);
		FKResource CastChanceResource = FKResource("CastChance", CastChanceModifiers, "Casting Chance", CastChanceValue, CastChanceValue, false, false);

		// Experience Boost
		TArray<FKResourceModifier> ExperienceBoostModifiers = {};
		FKResource ExperienceBoostResource = FKResource("ExperienceBoost", CastChanceModifiers, "Experience Boost", 0.f, 0.f, false, false);

		Resources = {	ManaResource , StaminaResource, PhysicalDamageResource, MagicalDamageResource,
						PhysicalDefenseResource, MagicalDefenseResource, DodgeChanceResource, CriticalChanceResource,
						CriticalDamageResource, CastChanceResource, ExperienceBoostResource };
	};

	void InitStats()
	{
		Stats = {
			FKStat("Vitality", 10, 10, FText::FromString("Vitality")),
			FKStat("Strength", 10, 10, FText::FromString("Strength")),
			FKStat("Dexterity", 10, 10, FText::FromString("Dexterity")),
			FKStat("Magic", 10, 10, FText::FromString("Magic")),
			FKStat("Intelligence", 10, 10, FText::FromString("Intelligence")),
			FKStat("Luck", 10, 10, FText::FromString("Luck"))
		};
	};

	void InitStatLevelPoints()
	{
		StatLevelPoints = {
			FKStatLevelPoint("Vitality"),
			FKStatLevelPoint("Strength"),
			FKStatLevelPoint("Dexterity"),
			FKStatLevelPoint("Magic"),
			FKStatLevelPoint("Intelligence"),
			FKStatLevelPoint("Luck")
		};
	};

	void InitResistances()
	{
		Resistances = {
			FKResistance("Fire", 0.f),
			FKResistance("Poison", 0.f),
			FKResistance("Ice", 0.f),
			FKResistance("Lightning", 0.f),
			FKResistance("Arcane", 0.f),
			FKResistance("Demonic", 0.f),
			FKResistance("Earth", 0.f)
		};
	};

	void InitElementalDamages()
	{
		ElementalDamages = 
		{
			FKElementalDamage("Fire", 0.f),
			FKElementalDamage("Poison", 0.f),
			FKElementalDamage("Ice", 0.f),
			FKElementalDamage("Lightning", 0.f),
			FKElementalDamage("Arcane", 0.f),
			FKElementalDamage("Demonic", 0.f),
			FKElementalDamage("Earth", 0.f)
		};
	};

	void InitData()
	{
		InitStats();
		InitStatLevelPoints();
		InitResources();
		InitResistances();
	};
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