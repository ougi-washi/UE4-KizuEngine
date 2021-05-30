// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Combat/KizuCombat.h"
#include "Core/KCharacter.h"
#include "KBuff.generated.h"

UENUM(BlueprintType)
enum class EBuffDamageValueType : uint8
{
	BDV_Static UMETA(DisplayName = "Static"),
	BDV_DynamicOnSource UMETA(DisplayName = "Dynamic On Source")
};

USTRUCT(BlueprintType)
struct FBuffEffect 
{
	GENERATED_USTRUCT_BODY()

public: 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff|Effect")
	TEnumAsByte<EResourceEffectType> ResourceEffectType = EResourceEffectType::Consumption;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff|Effect")
	bool bHealthResource = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|Spawnable Ability|Effect")
	FString ResourceName = "None";

	/** If the amount of damage or effect to apply is static or dynamic on a specific resource*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect)
	EBuffDamageValueType EffectValueType = EBuffDamageValueType::BDV_Static;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect, meta = (EditCondition = "EffectValueType != EBuffDamageValueType::BDV_Static"))
	FString SourceResourceName = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff|Effect")
	float Value = 10.f;
};

USTRUCT(BlueprintType)
struct FBuffData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	FString Name = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	float Duration = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	float TickingRate = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	float TickingDelay = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	bool bAttachToTargetActor = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bAttachToTargetActor"), Category = "Kizu|Buff")
	FName SocketName = "spine_02";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	TArray<FBuffEffect> BuffEffects;

	/** This will affect the movement of the targeted actor. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Kizu|Buff")
	bool bUseTimeDilation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, meta = (EditCondition = "bUseTimeDilation"),Category = "Kizu|Buff")
	FTimeDilationParams TimeDilationParams;

	/** This will affect the movement of the targeted actor. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "Kizu|Buff")
	bool bUseMovementSpeedModifier = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff", meta = (EditCondition = "bUseMovementSpeedModifier"))
	float MovementSpeedModifier = -100.f;
};

class AKCharacter;

UCLASS()
class KIZUENGINE_API AKBuff : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AKBuff();
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Buff")
	FBuffData BuffData;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Kizu|Buff")
	AActor* OwnerActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Kizu|Buff")
	AActor* TargetActor = nullptr;

	FTimerHandle TickingTimerHandle;
	FTimerHandle DurationHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called on the tick of the buff (The Effects will be applied here).
	UFUNCTION()
	void TriggerTicking();
	// Called when the duration of the Buff is reached.
	UFUNCTION()
	void TriggerDurationEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Buff")
	void OnBuffTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Buff")
	void OnBuffEnd();

	UFUNCTION(Server, BlueprintCallable, Reliable, Category = "Kizu|Buff")
	void ServerSetTargetActor(AActor* InTargetActor);

	UFUNCTION(Server, BlueprintCallable, Reliable, Category = "Kizu|Buff")
	void ServerSetOwnerActor(AActor* InOwnerActor);

	UFUNCTION(Server, BlueprintCallable, Unreliable, Category = "Kizu|Buff")
	void ServerAttachToTargetActor(const FName& SocketName);

	UFUNCTION(NetMulticast, BlueprintCallable, Unreliable, Category = "Kizu|Buff")
	void MulticastAttachToTargetActor(const FName& SocketName);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff|Effect")
	void ExecuteAllBuffEffects();

	UFUNCTION(BlueprintCallable, Category = "Kizu|Buff|Effect")
	void ExecuteBuffEffectOnCharacter(FBuffEffect& BuffEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Buff|Effect")
	void OnExecuteBuff(AKCharacter* AffectedCharacter, FBuffEffect BuffEffect);
	virtual void OnExecuteBuff_Native(AKCharacter* AffectedCharacter, FBuffEffect BuffEffect);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Buff|Effect")
	float GetEffectValue(const FBuffEffect& BuffEffect);
};
