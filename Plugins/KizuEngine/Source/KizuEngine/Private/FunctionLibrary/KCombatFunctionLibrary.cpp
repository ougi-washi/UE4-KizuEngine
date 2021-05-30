// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/KCombatFunctionLibrary.h"
#include "Engine/Engine.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"
#include "Core/Combat/KBuff.h"
#include "Core/Combat/KSpawnableAbility.h"
#include "Core/Extra/KRespawnTargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

AKBuff* UKCombatFunctionLibrary::SpawnBuff(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform)
{
	if (OwnerActor && TargetActor) {
		if (UWorld* World = OwnerActor->GetWorld()) {
			AKBuff* NewBuff = World->SpawnActorDeferred<AKBuff>(BuffToSpawn, Transform, OwnerActor, Cast<APawn>(OwnerActor));
			if (NewBuff) {
				NewBuff->ServerSetOwnerActor(OwnerActor);
				NewBuff->ServerSetTargetActor(TargetActor);
				UGameplayStatics::FinishSpawningActor(NewBuff, NewBuff->GetTransform());
				return NewBuff;
			}
		}
	}
	else UE_LOG(LogKizu, Warning, TEXT("Unable to spawn the Buff due to invalid Owner or Target."));
	return nullptr;
}

AKSpawnableAbility* UKCombatFunctionLibrary::SpawnSpawnableAbility(AActor* OwnerActor, TSubclassOf<AKSpawnableAbility> SpawnableAbilityToSpawn, FSpawnableAbilitySpawnParams SpawnParams)
{
	if (OwnerActor->IsValidLowLevel()) {
		if (UWorld* World = OwnerActor->GetWorld()) {
			AKSpawnableAbility* SpawnableAbility = World->SpawnActorDeferred<AKSpawnableAbility>(SpawnableAbilityToSpawn, SpawnParams.Transform, OwnerActor, Cast<APawn>(OwnerActor));
			if (SpawnableAbility->IsValidLowLevel()) {
				SpawnableAbility->ServerSetOwner(OwnerActor);
				UGameplayStatics::FinishSpawningActor(SpawnableAbility, SpawnableAbility->GetTransform());
				if (SpawnParams.bInitizalizeMobility) {
					SpawnableAbility->ServerInitializeMovement(SpawnParams.InitialDirection, SpawnParams.TargetActor);
				}
				return SpawnableAbility;
			}
		}
	}
	return nullptr;
}

bool UKCombatFunctionLibrary::IsSameFaction(AKCharacter* CharacterOne, AKCharacter* CharacterTwo)
{
	return CharacterOne->CharacterData.Faction == CharacterTwo->CharacterData.Faction;
}

bool UKCombatFunctionLibrary::IsSameFactionWithCast(AKCharacter* CharacterOne, AActor* CharacterTwo)
{
	if (AKCharacter* KCharacterTwo = Cast<AKCharacter>(CharacterTwo)) {
		return CharacterOne->CharacterData.Faction == KCharacterTwo->CharacterData.Faction;
	}
	return false;
}

AKRespawnTargetPoint* UKCombatFunctionLibrary::GetClosestRespawnTargetPoint(AKCharacter* Character, const FString PlayerID)
{
	if (Character)
	{
		TArray<AActor*> TargetPoints;
		UGameplayStatics::GetAllActorsOfClass(Character, AKRespawnTargetPoint::StaticClass(), TargetPoints);

		AKRespawnTargetPoint* ClosestTargetPoint = nullptr;
		double MinDistance = 100000000.f;
		for (AActor* TempTargetPoint : TargetPoints)
		{
			if (AKRespawnTargetPoint* TempRespawnTargetPoint = Cast<AKRespawnTargetPoint>(TempTargetPoint))
			{
				// Check if there is a respawn specific for this Player or use the default "DEFAULT" player ID.
				if ((TempRespawnTargetPoint->PlayerID == PlayerID) || (TempRespawnTargetPoint->PlayerID == "DEFAULT"))
				{
					// Check if there is a respawn specific for the faction of this player or use the default -1 value as a faction.
					if ((TempRespawnTargetPoint->Faction == Character->CharacterData.Faction) || TempRespawnTargetPoint->Faction == -1)
					{
						double CurrentDistance = Character->GetDistanceTo(TempRespawnTargetPoint);
						if (CurrentDistance < MinDistance)
						{
							ClosestTargetPoint = TempRespawnTargetPoint;
							MinDistance = CurrentDistance;
						}
					}
				}
			}
		}
		return ClosestTargetPoint;
	}
	return nullptr;
}

bool UKCombatFunctionLibrary::GetClosestRespawnTargetPointLocRot(FVector& OutLocation, FRotator& OutRotation, AKCharacter* Character, const FString PlayerID, const FVector LocationOffset)
{
	if (Character)
	{
		if (AKRespawnTargetPoint* Respawntarget = GetClosestRespawnTargetPoint(Character, PlayerID))
		{
			OutLocation = Respawntarget->GetActorLocation() + LocationOffset;
			OutRotation = Respawntarget->GetActorRotation();
			return true;
		}
		OutLocation = Character->GetActorLocation() + LocationOffset;
		OutRotation = Character->GetActorRotation();
	}
	return false;
}

TSubclassOf<UDamageType> UKCombatFunctionLibrary::GetDamageType(FDamageEvent &DamageEvent)
{
	return DamageEvent.DamageTypeClass;
}

void UKCombatFunctionLibrary::GetVariance(float& OutMin, float& OutMax, const float InValue /*= 200.f*/, const float InVariancePercentage /*= 5.f*/)
{
	float VarianceValue = (InValue * InVariancePercentage) / 100.f;
	OutMax = InValue + VarianceValue;
	OutMin = InValue - VarianceValue;
	if (OutMin < 1.f)
	{
		OutMin = 1.f;
	}
}
