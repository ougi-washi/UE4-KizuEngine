// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KCombatCharacter.h"
#include "Engine/DataTable.h"
#include "Core/Combat/KAbility.h"

bool AKCombatCharacter::ExecuteAbility(const FAbilityData &AbilityData)
{
	bool bHasEnoughResource = HasEnoughResource(AbilityData.ResourceName, AbilityData.Value);

	// Consume resources
	if (ConsumeResource(AbilityData.ResourceName, AbilityData.Value, true)) {
		//ConsumeResource(AbilityData.ResourceName, AbilityData.Value);
		MontagePlay_Replicated(AbilityData.AnimMontage, 1.f);
		return true;
	}
	return false;
}

bool AKCombatCharacter::ExecuteAbilityFromDT(const UDataTable* DataTable, const FString AbilityID)
{
	//DataTable->GetRow()
	return false;
}
