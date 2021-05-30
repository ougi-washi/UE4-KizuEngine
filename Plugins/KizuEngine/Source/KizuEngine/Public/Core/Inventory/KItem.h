// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Combat/KizuCombat.h"
#include "Engine/DataTable.h"
#include "KItem.generated.h"

class AKBuff;


USTRUCT(BlueprintType)
struct FKItemEquipStatBonus {

	GENERATED_BODY()

public:

	/** The stat name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	FString Name;

	/** The value to affect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	float Value;

	FKItemEquipStatBonus()
	{
		// Default Constructor
		Name = "None";
		Value = 0.f;
	};

	FKItemEquipStatBonus(const FString& NewName, const float NewValue = 0.f)
	{
		Name = NewName;
		Value = NewValue;
	};

};

USTRUCT(BlueprintType)
struct FKItemEquipBonus {

	GENERATED_BODY()

public:

	/** The resource name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	FString Name;

	/** The value to affect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	float Value;

	FKItemEquipBonus()
	{
		// Default Constructor
		Name = "None";
		Value = 0.f;
	};

	FKItemEquipBonus(const FString& NewName, const float NewValue = 0.f)
	{
		Name = NewName;
		Value = NewValue;
	};
};

USTRUCT(BlueprintType)
struct FKItemUseResourceBonus {

	GENERATED_BODY()

public:

	/** The resource name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	FString Name = "None";

	/** The value to affect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	float Value = 5.f;

	/** The buff/debuff to apply */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TSubclassOf<AKBuff> Buff;

	/** The effect type */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Gain;
};

USTRUCT(BlueprintType)
struct FKItemEffect {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TArray<FKItemEquipBonus> ItemEquipStatBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TArray<FKItemEquipBonus> ItemEquipResourceBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TArray<FKItemEquipBonus> ItemEquipResistanceBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TArray<FKItemUseResourceBonus> UseResourceBonuses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	TArray<FString> ActionsToLearn;

	FKItemEffect()
	{

		// Default Constructor

		ItemEquipStatBonuses = {
			FKItemEquipBonus("Vitality", 0.f),
			FKItemEquipBonus("Strength", 0.f),
			FKItemEquipBonus("Dexterity", 0.f),
			FKItemEquipBonus("Magic", 0.f),
			FKItemEquipBonus("Intelligence", 0.f),
			FKItemEquipBonus("Luck", 0.f)
		};

		ItemEquipResourceBonuses = {
			FKItemEquipBonus("Mana", 0.f),
			FKItemEquipBonus("Stamina", 0.f),
			FKItemEquipBonus("PhysicalDamage", 0.f),
			FKItemEquipBonus("MagicalDamage", 0.f),
			FKItemEquipBonus("PhysicalDefense", 0.f),
			FKItemEquipBonus("MagicalDefense", 0.f),
			FKItemEquipBonus("DodgeChance", 0.f),
			FKItemEquipBonus("CriticalChance", 0.f),
			FKItemEquipBonus("CriticalDamage", 0.f),
			FKItemEquipBonus("ExperienceBoost", 0.f)
		};

		ItemEquipResistanceBonuses = {
			FKItemEquipBonus("Fire", 0.f),
			FKItemEquipBonus("Poison", 0.f),
			FKItemEquipBonus("Ice", 0.f),
			FKItemEquipBonus("Lightning", 0.f),
			FKItemEquipBonus("Arcane", 0.f),
			FKItemEquipBonus("Demonic", 0.f),
			FKItemEquipBonus("Earth", 0.f)
		};
	};

};

USTRUCT(BlueprintType)
struct FKItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = "ItemName";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FKItemEffect> UsageEffects;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bUseStaticMesh : 1 ;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUseStaticMesh"))
	UStaticMesh* StaticMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bUseStaticMesh"))
	USkeletalMesh* SkeletalMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	float Weight = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	int Price = 1;

	FKItemData() : FTableRowBase() {
		bUseStaticMesh = 1;
	}
};
// FKItemData == operator 
inline bool operator==(const FKItemData& lhs, const FKItemData& rhs)
{
	return lhs.Id == rhs.Id;
}

UCLASS()
class KIZUENGINE_API AKItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKItem();

	/** Item data related to it, it represents its name, mesh and other. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKItemData ItemData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
