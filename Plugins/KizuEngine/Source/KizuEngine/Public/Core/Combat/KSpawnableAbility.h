// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/Combat/KizuCombat.h"
#include "GameFramework/Actor.h"
#include "Core/KAction.h"
#include "KSpawnableAbility.generated.h"

class USceneComponent;
class USphereComponent;
class UProjectileMovementComponent;
class AKCharacter;
class AKBuff;

UENUM(BlueprintType)
enum ESpawnableAbilityTriggerType
{
	OnSpawn,
	OnHit
};

USTRUCT(BlueprintType)
struct FSpawnableAbilityEffect
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	TEnumAsByte<ESpawnableAbilityTriggerType> SpawnableAbilityTriggerType = ESpawnableAbilityTriggerType::OnHit;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Consumption;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	bool bHealthResource = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|Spawnable Ability|Effect")
	FString ResourceName = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	float Value = 10.f;
	/** The effects that are going to be executed on the trigger event of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	TArray<TSubclassOf<AKBuff>> Buffs;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	bool bAffectOtherFaction = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	bool bAffectOwnerFaction = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability|Effect")
	bool bAffectOwner = false;
	/** Reaction data to apply on Effect trigger*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data|Reaction")
	FReactionSendingData ReactionSendingData;
};

USTRUCT(BlueprintType)
struct FSpawnableAbilityData
{
	GENERATED_USTRUCT_BODY()

public:

	/** The name of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data")
	FString Name = "None";
	/** If the SpawnableAbility is custom or relying on the base preset.*/
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Spawnable Ability")
	//bool bUsePreset = true;
	/** The effects that are going to be executed on the trigger event of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = "Kizu|Data|Effect")
	TArray<FSpawnableAbilityEffect> Effects;
	/** If the SpawnableAbility will affect the target only once. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data|Effect")
	bool bAffectOnce = true;
	/** If the SpawnableAbility is going to tick the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data|Effect")
	bool bTickEffects = false;
	/** The ticking rate of the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bTickEffects"), Category = "Kizu|Data|Effect")
	float TickingRate = 1.f;
	/** Set the Spawnable Ability to be destroyed on collision event after applying the effects */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Data|Destroy")
	uint8 bDestroyOnHit : 1;
	/** The timer that will represent the end of the spawnable ability after calling "Destroy Spawnable". */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bDestroyOnHit"), Category = "Kizu|Data|Destroy", AdvancedDisplay)
	float DestroyTimer = 2.f;

	FSpawnableAbilityData() {
		bDestroyOnHit = true;
	}
};

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
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Spawnable Ability|Collision")
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

	UFUNCTION(BlueprintCallable, Category = "Kizu|Spawnable Ability|Effect")
	void TriggerDestroytimer(const float DestroyTimer = 2.f);
};
