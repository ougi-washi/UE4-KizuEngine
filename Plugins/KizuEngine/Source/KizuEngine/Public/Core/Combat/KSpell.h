// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/Combat/KizuCombat.h"
#include "GameFramework/Actor.h"
#include "KSpell.generated.h"

class USceneComponent;
class USphereComponent;
class AKCharacter;
class AKBuff;

UENUM(BlueprintType)
enum ESpellTriggerType
{
	OnSpawn,
	OnHit
};

USTRUCT(BlueprintType)
struct FSpellEffect
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	TEnumAsByte<ESpellTriggerType> SpellTriggerType = ESpellTriggerType::OnHit;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Consumption;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	bool bHealthResource = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|Spell|Effect")
	FString ResourceName = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	float Value = 10.f;
	/** The effects that are going to be executed on the trigger event of the Spell.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Effect")
	TArray<TSubclassOf<AKBuff>> Buffs;
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
	FString Name = "None";
	/** If the Spell is custom or relying on the base preset.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell")
	bool bUsePreset = true;
	/** The effects that are going to be executed on the trigger event of the Spell.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = "Kizu|Spell")
	TArray<FSpellEffect> Effects;
	/** If the Spell will affect the target only once. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell")
	bool bAffectOnce = true;
	/** If the spell is going to tick the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell")
	bool bTickEffects = false;
	/** The ticking rate of the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bTickEffects"), Category = "Kizu|Spell")
	float TickingRate = 1.f;
};


UCLASS()
class KIZUENGINE_API AKSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKSpell();
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Shape component used for collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spell|Data")
	FSpellData SpellData;

	UPROPERTY(BlueprintReadWrite, Category = "Kizu|Spell|Data")
	TArray<AActor*> AffectedActors;

	FTimerHandle TickingTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void TriggerTicking();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	TArray<FSpellEffect> GetAllOnSpawnEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	TArray<FSpellEffect> GetAllOnHitEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spell|Collision")
	bool GetOverlappingActorsByTag(TArray<AActor*> &OverlappingActors, const FName PrimitiveComponentTag);

	UFUNCTION()
	virtual void OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spell|Collision")
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	void ExecuteSpellEffectByCollision(FSpellEffect &SpellEffect, UPrimitiveComponent* inCollisionComponent);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	void ExecuteSpellEffectOnCharacter(FSpellEffect &SpellEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Effect")
	void ExecuteSpellEffects(TArray<FSpellEffect> SpellEffects);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Spell|Buff")
	void ServerSpawnBuff(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Buff")
	void ExecuteBuffOnCharacter(TSubclassOf<AKBuff> Buff, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spell|Buff")
	void ExecuteBuffsOnCharacter(TArray<TSubclassOf<AKBuff>> Buffs, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Server, Unreliable,Category = "Kizu|Spell|Effect")
	void ServerResetAffectedActors();

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spell|Effect")
	void OnFinishExecuteSpellEffects();
	virtual void OnFinishExecuteSpellEffects_Native();
};
