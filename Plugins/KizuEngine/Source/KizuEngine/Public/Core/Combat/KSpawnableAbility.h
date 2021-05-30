// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/Combat/KizuCombat.h"
#include "GameFramework/Actor.h"
#include "Core/KAction.h"
#include "Core/Combat/KSpawnableAbilityLibrary.h"
#include "KSpawnableAbility.generated.h"

class USceneComponent;
class USphereComponent;
class UProjectileMovementComponent;
class AKCharacter;

UCLASS()
class KIZUENGINE_API AKSpawnableAbility : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AKSpawnableAbility();

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Shape component used for collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	/** Projectile movement component that controls the movement of the projectile (Speed, Homing or not homing)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data")
	FSpawnableAbilityData SpawnableAbilityData;

	UPROPERTY(BlueprintReadWrite, Category = "Kizu|Data")
	TArray<AActor*> AffectedActors;
	/** Is true after colliding with something */
	UPROPERTY()
	uint8 bHasHit : 1;

	FTimerHandle TickingTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TriggerTicking();

	/** Initiate the SpawnableAbility movement data by setting its target actor to home towards and Initial direction to head to. (Does not replicate)
	 * @param InitialDirection The initial direction that the SpawnableAbility will be launched into.
	 * @param TargetActor The target actor to home towards.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	virtual void InitializeMovement(const FVector InitialDirection, AActor* TargetActor);

	/** Initiate the SpawnableAbility movement data by setting its target actor to home towards and Initial direction to head to. (On the server side and calls NetMulticast)
	 * @param InitialDirection The initial direction that the SpawnableAbility will be launched into.
	 * @param TargetActor The target actor to home towards.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	virtual void ServerInitializeMovement(const FVector InitialDirection, AActor* TargetActor);

	/** Initiate the SpawnableAbility movement data by setting its target actor to home towards and Initial direction to head (On all clients)
	 * @param InitialDirection The initial direction that the SpawnableAbility will be launched into.
	 * @param TargetActor The target actor to home towards.
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	virtual void MulticastInitializeMovement(const FVector InitialDirection, AActor* TargetActor);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|General")
	void ServerSetOwner(AActor* InOwnerActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spawnable Ability|Collision")
	TArray<FSpawnableAbilityEffect> GetAllOnSpawnEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spawnable Ability|Collision")
	TArray<FSpawnableAbilityEffect> GetAllOnHitEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spawnable Ability|Collision")
	bool GetOverlappingActorsByTag(TArray<AActor*> &OverlappingActors, const FName PrimitiveComponentTag);

	UFUNCTION()
	virtual void OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/**
	 * Collision
	 */

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spawnable Ability|Collision")
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Collision")
	void Multicast_SetActorEnableCollision(bool bNewValue);

	/**
	 * Movement
	 */

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Spawnable Ability|Movement")
	bool CanHomeToTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	void SnapToGround();

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	bool AttachToOwnerCharacterMesh(const FName SocketName = "None");

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Movement")
	bool AttachToOwner();

	/**
	 * Execute Spawnable Ability
	 */

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	float GetEffectValue(FSpawnableAbilityEffect& SpawnableAbilityEffect);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void ExecuteSpawnableAbilityEffectByCollision(FSpawnableAbilityEffect &SpawnableAbilityEffect, UPrimitiveComponent* inCollisionComponent);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void ExecuteSpawnableAbilityEffectOnCharacter(FSpawnableAbilityEffect &SpawnableAbilityEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void ExecuteSpawnableAbilityEffects(TArray<FSpawnableAbilityEffect> SpawnableAbilityEffects);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Buff")
	void ServerSpawnBuff(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Buff")
	void ExecuteBuffOnCharacter(TSubclassOf<AKBuff> Buff, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Buff")
	void ExecuteBuffsOnCharacter(TArray<TSubclassOf<AKBuff>> Buffs, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(Server, Unreliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void ServerResetAffectedActors();

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spawnable Ability|Effect")
	void OnFinishExecuteSpawnableAbilityEffects();
	virtual void OnFinishExecuteSpawnableAbilityEffects_Native();

	/**
	 * Destruction
	 */

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void TriggerDestroyTimer(const float DestroyTimer = 2.f);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spawnable Ability|Effect")
	void OnStartDestruction();
	void OnStartDestruction_Native();

	/**
	 * Effect
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spawnable Ability|Effect")
	void OnApplyEffectToCharacter(AKCharacter* TargetCharter);
	void OnApplyEffectToCharacter_Native(AKCharacter* TargetCharter);

	/**
	 * Emitters
	 */
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Emitter")
	void ServerSpawnEmitter(UParticleSystem* EmitterTemplate);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Spawnable Ability|Emitter")
	void MulticastSpawnEmitter(UParticleSystem* EmitterTemplate);

};
