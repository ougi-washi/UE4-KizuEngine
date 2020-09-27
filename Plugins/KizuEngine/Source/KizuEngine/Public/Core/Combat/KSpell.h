// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KSpell.generated.h"


UENUM(BlueprintType)
enum ESpellTriggerType
{
	OnSpawn,
	OnHit
};

UENUM(BlueprintType)
enum EResourceEffectType
{
	Gain,
	Consumption
};

USTRUCT(BlueprintType)
struct FSpellEffect
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	TEnumAsByte<ESpellTriggerType> SpellTriggerType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Consumption;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	bool bHealthResource = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|Spell|Effect")
	FString ResourceName = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	float Value = 10.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	bool bAffectOtherFaction = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	bool bAffectOwnerFaction = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	bool bAffectOwner = false;
};

USTRUCT(BlueprintType)
struct FSpellData
{
	GENERATED_USTRUCT_BODY()

public:

	/** The name of the Spell.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell")
	FString Name;
	/** If the Spell is custom.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell")
	bool bUsePreset = true;
	/** The effects that are going to be executed on the trigger event of the Spell.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = "Kizu|Spell")
	TArray<FSpellEffect> Effects;
};

class USphereComponent;
class AKCharacter;

UCLASS()
class KIZUENGINE_API AKSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKSpell();

	/** Shape component used for collision */
	UPROPERTY(Category = TriggerBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Data")
	FSpellData SpellData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	TArray<FSpellEffect> GetAllOnSpawnEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	TArray<FSpellEffect> GetAllOnHitEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	bool GetOverlappingActorsByTag(TArray<AActor*> &OverlappingActors, const FName PrimitiveComponentTag);

	UFUNCTION()
	virtual void OnCollisionBeingOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spell|Collision")
	void OnCollisionBeingOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	static void ExecuteSpellEffectByCollision(FSpellEffect SpellEffect, UPrimitiveComponent* inCollisionComponent);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	static void ExecuteSpellEffectOnCharacter(FSpellEffect SpellEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	void ExecuteSpellEffects(TArray<FSpellEffect> SpellEffects);
};
