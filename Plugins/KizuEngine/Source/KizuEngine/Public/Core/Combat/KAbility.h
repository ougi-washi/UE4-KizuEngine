// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KAbility.generated.h"

UENUM(BlueprintType)
enum EAbilityTriggerType
{
	OnSpawn,
	OnHit
};

USTRUCT(BlueprintType)
struct FAbilityEffect
{

	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	TEnumAsByte<EAbilityTriggerType> AbilityTriggerType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	bool bHealthResource = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bHealthResource"), Category = "Kizu|Ability|Effect")
	FName ResourceName = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	float Value = 10.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	bool bAffectionOtherFaction = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	bool bAffectOwnerFaction = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability|Effect")
	bool bAffectOwner = false;
};

USTRUCT(BlueprintType)
struct FAbility
{
	GENERATED_USTRUCT_BODY()

public:

	/** The name of the ability.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	FString Name;
	/** If the ability is custom.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Ability")
	bool bUsePreset = true;
	/** The effects that are going to be executed on the trigger event of the Ability.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = "Kizu|Ability")
	TArray<FAbilityEffect> Effects;
	/** The Cooldown of the ability.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUsePreset"), Category = "Kizu|Ability")
	float Cooldown = 5.f;

};

UCLASS()
class KIZUENGINE_API AKAbility : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKAbility();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
