// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KAbility.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"

// Sets default values
AKAbility::AKAbility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AKAbility::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKAbility::OnCooldownReady_Native()
{
	OnCooldownReady();
	if (AbilityData.DestroyOnCooldownReady) {
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
		Destroy(true, true);
	}
}

bool AKAbility::ExecuteAbility()
{
	AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());
	if (OwnerCharacter) {
		bool bHasEnoughResource = false;
		// Checks the player resources
		if (AbilityData.ResourceSelection.bHealthResource) 
			bHasEnoughResource = OwnerCharacter->HasEnoughHealth(AbilityData.ResourceSelection.Value);
		else bHasEnoughResource = OwnerCharacter->HasEnoughResource(AbilityData.ResourceSelection.ResourceName, AbilityData.ResourceSelection.Value);
		// Consume resources
		if (bHasEnoughResource) {
			if (AbilityData.ResourceSelection.bHealthResource)
				OwnerCharacter->ConsumeResource(AbilityData.ResourceSelection.ResourceName, AbilityData.ResourceSelection.Value);
			else OwnerCharacter->ServerApplyDamage(OwnerCharacter, AbilityData.ResourceSelection.Value, NULL);
			OwnerCharacter->MontagePlay_Replicated(AbilityData.AnimMontage, 1.f);
			GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AKAbility::OnCooldownReady_Native, .1f, false, AbilityData.Cooldown);
			return true;
		}
	}
	return false;
}

