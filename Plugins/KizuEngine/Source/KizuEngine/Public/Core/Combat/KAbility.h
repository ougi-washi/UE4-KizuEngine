// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Core/Combat/KizuCombat.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "KAbility.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FAbilityData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** Name of the ability */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString Name = "None";
	/** Animation Montage to play for this Ability */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	UAnimMontage* AnimMontage;
	/** Resource name to use. In order to consume Health as resource, please use "DEFAULT_HEALTH" */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString ResourceName = "None";
	/** Value of the resources to consume */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Value = 10;
	/** Cooldown of the ability to be able to use it again */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Cooldown = 5.f;
	/** Destroy the object when the CoolDown is ready (advanced usage) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Advanced)
	bool DestroyOnCooldownReady = true;
};

class AKCharacter;

UCLASS(Abstract)
class KIZUENGINE_API AKAbility : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKAbility();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Data")
	FAbilityData AbilityData;

	FTimerHandle CooldownTimer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCooldownReady_Native();
	UFUNCTION(BlueprintImplementableEvent, Category="Kizu|Ability")
	void OnCooldownReady();
	/**
	 * Execute the ability depending on its data.
	 * In order to make it work properly, the owner has to be a KCharacter.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Ability")
	bool ExecuteAbility();

};
