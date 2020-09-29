// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Combat/KizuCombat.h"
#include "KAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	FString Name = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	UAnimMontage* AnimMontage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	FResourceSelection ResourceSelection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	float Cooldown = 5.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	bool DestroyOnCooldownReady = true;
};

UCLASS()
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
