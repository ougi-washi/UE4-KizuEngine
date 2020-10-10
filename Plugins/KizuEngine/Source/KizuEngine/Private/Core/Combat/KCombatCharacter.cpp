// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KCombatCharacter.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "Core/Combat/KAbility.h"

bool AKCombatCharacter::ExecuteAbility(const FAbilityData &AbilityData, const bool bUseCooldown)
{
	FCooldown Cooldown;
	float TimeElapsed;
	float TimeRemaining;
	if (GetCooldownTimer(AbilityData.Name, TimeElapsed, TimeRemaining) && bUseCooldown) {
		OnNotifyCooldown_Native(Cooldown.ID, TimeElapsed, TimeRemaining);
		return false;
	}
	else if (ConsumeResource(AbilityData.ResourceName, AbilityData.Value, true)) {
		MontagePlay_Replicated(AbilityData.AnimMontage, 1.f);
		if (bUseCooldown) {
			Cooldown = FCooldown(AbilityData.Name, AbilityData.Cooldown);
			StartCooldown(Cooldown);
		}
		return true;
	}
	return false;
}

bool AKCombatCharacter::AddCooldownToStack(const FCooldown& Cooldown)
{
	if (CooldownStack.Contains(Cooldown))
		return false;
	CooldownStack.AddUnique(Cooldown);
	return true;
}

bool AKCombatCharacter::StartCooldown(FCooldown& Cooldown)
{
	if (UWorld* World = GetWorld()) {
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("EndCooldown"), Cooldown.ID);
		World->GetTimerManager().SetTimer(Cooldown.TimerHandle, TimerDel, 1.f, true, Cooldown.Duration);
		AddCooldownToStack(Cooldown);
		return true;
	}
	return false;
}

void AKCombatCharacter::EndCooldown(const FString CooldownID)
{
	FCooldown Cooldown(CooldownID);
	int32 CooldownIndex;
	if (GetCooldown(CooldownID, Cooldown, CooldownIndex))
	{
		if (UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(Cooldown.TimerHandle);
		CooldownStack.RemoveAt(CooldownIndex, 1, true);
		OnEndCooldown(Cooldown);
	}
}

bool AKCombatCharacter::GetCooldown(const FString InID, FCooldown& OutCooldown)
{
	// Lossy method for BPs
	int32 Index;
	return GetCooldown(InID, OutCooldown, Index);
}

bool AKCombatCharacter::GetCooldown(const FString InID, FCooldown& OutCooldown, int32& Index)
{
	OutCooldown = FCooldown(InID);
	Index = CooldownStack.Find(OutCooldown);
	if (!CooldownStack.IsValidIndex(Index))
		return false;
	OutCooldown = CooldownStack[Index];
	return true;
}

bool AKCombatCharacter::GetCooldownTimer(const FString InCooldownID, float& Elapsed, float& Remaining)
{
	FCooldown Cooldown;
	if (GetCooldown(InCooldownID, Cooldown))
	{
		if (UWorld* World = GetWorld()) {
			Elapsed = World->GetTimerManager().GetTimerElapsed(Cooldown.TimerHandle);
			Remaining = World->GetTimerManager().GetTimerRemaining(Cooldown.TimerHandle);
			return true;
		}
	}
	Elapsed = -1;
	Remaining = -1;
	return false;
}

void AKCombatCharacter::OnNotifyCooldown_Native(const FString& CooldownID, const float& Elapsed, const float& Remaining)
{
	OnNotifyCooldown(CooldownID, Elapsed, Remaining);
}
