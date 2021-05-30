// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/Combat/KizuCombat.h"
#include "GameFramework/Actor.h"
#include "Core/KAction.h"
#include "KSpawnableAbilityLibrary.generated.h"

class AKBuff;
class UParticleSystem;
class UDamageType;

UENUM(BlueprintType)
enum ESpawnableAbilityTriggerType
{
	OnSpawn,
	OnHit
};

UENUM(BlueprintType)
enum class ESpawnableAbilityDamageValueType : uint8
{
	SADV_Static UMETA(DisplayName = "Static"),
	SADV_DynamicOnSource UMETA(DisplayName = "Dynamic On Source")
};

USTRUCT(BlueprintType)
struct FSpawnableAbilityEffect
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	FString Type = "Neutral";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	TEnumAsByte<ESpawnableAbilityTriggerType> SpawnableAbilityTriggerType = ESpawnableAbilityTriggerType::OnHit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Consumption;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bHealthResource = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = Effect)
	FString ResourceName = "None";

	/** If the amount of damage or effect to apply is static or dynamic on a specific resource*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	ESpawnableAbilityDamageValueType EffectValueType = ESpawnableAbilityDamageValueType::SADV_Static;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect, meta = (EditCondition = "EffectValueType != ESpawnableAbilityDamageValueType::SADV_Static"))
	FString SourceResourceName = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	float Value = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	TSubclassOf<UDamageType> DamageType = nullptr;

	/** The effects that are going to be executed on the trigger event of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	TArray<TSubclassOf<AKBuff>> Buffs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bAffectOtherFaction = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bAffectOwnerFaction = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bAffectOwner = false;

	/** Reaction data to apply on Effect trigger*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Reaction)
	FReactionSendingData ReactionSendingData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Emitter)
	uint8 bSpawnEmitterOnApply : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Emitter, meta = (EditCondition = "bSpawnEmitterOnApply"))
	UParticleSystem* ParticleSystem = nullptr;
};

USTRUCT(BlueprintType)
struct FSpawnableAbilityData
{
	GENERATED_BODY()

public:

	/** The name of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Data)
	FString Name = "None";

	/** The effects that are going to be executed on the trigger event of the SpawnableAbility.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = Effect)
	TArray<FSpawnableAbilityEffect> Effects;

	/** If the SpawnableAbility will affect the target only once. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bAffectOnce = true;

	/** If the SpawnableAbility is going to tick the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	bool bTickEffects = false;

	/** The ticking rate of the effects. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bTickEffects"), Category = Effect)
	float TickingRate = 1.f;

	/** Set the Spawnable Ability to be destroyed on collision event after applying the effects */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Destroy)
	uint8 bDestroyOnHit : 1;

	/** The timer that will represent the end of the spawn-able ability after calling "Destroy Spawnable". */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bDestroyOnHit"), Category = Destroy, AdvancedDisplay)
	float DestroyTimer = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter")
	uint8 bSpawnEmitterOnDestroy : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter", meta = (EditCondition = "bSpawnEmitterOnDestroy"))
	UParticleSystem* ParticleSystemOnDestroy = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter", meta = (EditCondition = "bSpawnEmitterOnDestroy"))
	FVector ScaleParticleOnDestroy = FVector(1.f, 1.f, 1.f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter")
	uint8 bSpawnEmitterOnSpawn : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter", meta = (EditCondition = "bSpawnEmitterOnSpawn"))
	UParticleSystem* ParticleSystemOnSpawn = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Emitter", meta = (EditCondition = "bSpawnEmitterOnSpawn"))
	FVector ScaleParticleOnSpawn = FVector(1.f, 1.f, 1.f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collision")
	uint8 bAffectOnlySkeletalMesh : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collision")
	uint8 bSnapToGround : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement)
	uint8 bAttachToOwner : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement)
	FName AttachSocketName = "None";

	FSpawnableAbilityData() {
		bDestroyOnHit = true;
		bAffectOnlySkeletalMesh = true;
	}
};