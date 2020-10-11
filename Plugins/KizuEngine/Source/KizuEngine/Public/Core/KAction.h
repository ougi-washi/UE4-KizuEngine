// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Core/Combat/KizuCombat.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "KAction.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FActionData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** Name of the Action. Make sure the Name of the Action is unique as it's the Key for the Cooldown Stack.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString Name = "None";
	/** Animation Montage to play for this Action */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	UAnimMontage* AnimMontage;
	/** Resource name to use. In order to consume Health as resource, please use "DEFAULT_HEALTH" */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString ResourceName = "None";
	/** Value of the resources to consume */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Value = 10;
	/** Cooldown of the Action to be able to use it again */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Cooldown = 5.f;
};


USTRUCT(BlueprintType)
struct FCooldown
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString ID = "None";
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float Duration = 0.f;

	FTimerHandle TimerHandle;

	FORCEINLINE bool operator==(const FCooldown& rhs) const
	{
		return (rhs.ID == this->ID);
	}
	FCooldown() {
		// Default Constructor
	}
	FCooldown(const FString& InID) {
		this->ID = InID;
	}
	FCooldown(const FString& InID, const float& InDuration) {
		this->ID = InID;
		this->Duration = InDuration;
	}

	static bool FindByID(const TArray<FCooldown>& InCooldowns, const FString& InID, FCooldown& OutCooldown) {
		for (int i = 0; i < InCooldowns.Num(); i++) {
			OutCooldown = InCooldowns[i];
			if (OutCooldown.ID == InID) {
				return true;
			}
		}
		return false;
	}
};

UCLASS(Abstract)
class KIZUENGINE_API AKAction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
