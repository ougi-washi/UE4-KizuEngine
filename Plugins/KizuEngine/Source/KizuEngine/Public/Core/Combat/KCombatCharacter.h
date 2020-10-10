// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Core/KCharacter.h"
#include "KCombatCharacter.generated.h"

struct FAbilityData;
class UDataTable;

/**
 * 
 */

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

	static bool FindByID(const TArray<FCooldown> &InCooldowns, const FString &InID, FCooldown& OutCooldown) {
		for (int i = 0; i < InCooldowns.Num(); i++) {
			OutCooldown = InCooldowns[i];
			if (OutCooldown.ID == InID) {
				return true;
			}
		}
		return false;
	}
};

UCLASS()
class KIZUENGINE_API AKCombatCharacter : public AKCharacter
{
	GENERATED_BODY()
	
private:

	bool AddCooldownToStack(const FCooldown& Cooldown);

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FCooldown> CooldownStack;

	UFUNCTION(BlueprintCallable, Category = "Kizu|Ability")
	bool ExecuteAbility(const FAbilityData &AbilityData, const bool bUseCooldown = true);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Cooldown")
	bool StartCooldown(UPARAM(ref) FCooldown& Cooldown);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Cooldown")
	void EndCooldown(const FString CooldownID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Cooldown", meta=(Keywords = "Ready"))
	void OnEndCooldown(const FCooldown& Cooldown);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Cooldown")
	bool GetCooldown(const FString InID, FCooldown& OutCooldown);
	bool GetCooldown(const FString InID, FCooldown& OutCooldown, int32 &Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Cooldown")
	bool GetCooldownTimer(const FString InCooldownID, float& Elapsed, float& Remaining);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Cooldown")
	void OnNotifyCooldown(const FString& CooldownID, const float& Elapsed, const float& Remaining);
	void OnNotifyCooldown_Native(const FString& CooldownID, const float& Elapsed, const float& Remaining);
};
