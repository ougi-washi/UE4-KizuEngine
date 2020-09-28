// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KSpell.h"
#include "KizuEngine.h"
#include "Core/Combat/KBuff.h"
#include "Net/UnrealNetwork.h"
#include "Core/KCharacter.h"
#include "FunctionLibrary/KCombatFunctionLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
AKSpell::AKSpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	if (CollisionComponent) {
		CollisionComponent->bHiddenInGame = false;
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKSpell::OnCollisionBeginOverlap_Native);
	}
}

void AKSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AKSpell, AffectedActors);
}

// Called when the game starts or when spawned
void AKSpell::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		ExecuteSpellEffects(GetAllOnSpawnEffects());
		if (SpellData.bTickEffects)
			GetWorld()->GetTimerManager().SetTimer(TickingTimerHandle, this, &AKSpell::TriggerTicking, SpellData.TickingRate, true);
	}
}

// Called every frame
void AKSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKSpell::TriggerTicking()
{
	ServerResetAffectedActors();
	ExecuteSpellEffects(GetAllOnHitEffects());
}

TArray<FSpellEffect> AKSpell::GetAllOnSpawnEffects()
{
	TArray<FSpellEffect> ResultEffects;
	for (FSpellEffect SpellEffect : SpellData.Effects)
	{
		if (SpellEffect.SpellTriggerType == ESpellTriggerType::OnSpawn)
			ResultEffects.Add(SpellEffect);
	}
	return ResultEffects;
}

TArray<FSpellEffect> AKSpell::GetAllOnHitEffects()
{
	TArray<FSpellEffect> ResultEffects;
	for (FSpellEffect SpellEffect : SpellData.Effects)
	{
		if (SpellEffect.SpellTriggerType == ESpellTriggerType::OnHit)
			ResultEffects.Add(SpellEffect);
	}
	return ResultEffects;
}

bool AKSpell::GetOverlappingActorsByTag(TArray<AActor*> &OverlappingActors, const FName PrimitiveComponentTag)
{
	TArray<UActorComponent*> ComponentsWithTag = GetComponentsByTag(UActorComponent::StaticClass(), PrimitiveComponentTag);
	
	for (UActorComponent* TriggerComponent : ComponentsWithTag)
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(TriggerComponent)) {
			TArray<AActor*> TempActors;
			PrimitiveComponent->GetOverlappingActors(TempActors);
			for (AActor* TempActor : TempActors)
			{
				if (TempActor!=this)
					OverlappingActors.AddUnique(TempActor);
			}
		}

	if (OverlappingActors.Num() > 0)
		return true;
	return false;
}

void AKSpell::OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
		ExecuteSpellEffects(GetAllOnHitEffects());
	OnCollisionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AKSpell::ExecuteSpellEffectByCollision(FSpellEffect &SpellEffect, UPrimitiveComponent* inCollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	inCollisionComponent->GetOverlappingActors(OverlappingActors);
	for (AActor* TempActor : OverlappingActors)
	{
		// Check if the spell is supposed to affect the overlapping actor only once. (This actor will be skipped in case it's not supposed to be affected again by this same effect)
		if (AffectedActors.Find(TempActor) > -1 && SpellData.bAffectOnce) {
			// Can't be affected
		}
		else {
			// Initiate effect on the actor
			AKCharacter* OtherCharacter = Cast<AKCharacter>(TempActor);
			AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());

			if (!OtherCharacter || !OwnerCharacter)
				return;

			if (SpellEffect.bAffectOwner && OtherCharacter == OwnerCharacter) {
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
			}
			if (SpellEffect.bAffectOtherFaction && OtherCharacter->CharacterData.Faction != OwnerCharacter->CharacterData.Faction) {
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
			}
			if ((SpellEffect.bAffectOwnerFaction && OtherCharacter->CharacterData.Faction == OwnerCharacter->CharacterData.Faction)
				&& (OtherCharacter != OwnerCharacter))
			{
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
			}
		}
	}		
}

void AKSpell::ExecuteSpellEffectOnCharacter(FSpellEffect &SpellEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (SpellEffect.ResourceEffectType == EResourceEffectType::Consumption) {
		if (SpellEffect.bHealthResource) {
			OwnerCharacter->ServerApplyDamage(TargetCharacter, SpellEffect.Value, NULL);
			UE_LOG(LogKizu, Log, TEXT("<%s> damages <%s> by <%f> with spell <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, SpellEffect.Value, *SpellData.Name);
		}
		else {
			TargetCharacter->ConsumeResource(SpellEffect.ResourceName, SpellEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> drains <%s>'s <%s> by <%f> with spell <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpellEffect.ResourceName, SpellEffect.Value, *SpellData.Name);
		}
	}
	if (SpellEffect.ResourceEffectType == EResourceEffectType::Gain) {
		if (SpellEffect.bHealthResource) {
			TargetCharacter->GainHealth(SpellEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> heals <%s> by <%f> with spell <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, SpellEffect.Value, *SpellData.Name);
		}
		else {
			TargetCharacter->GainResource(SpellEffect.ResourceName, SpellEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> recovers <%s>'s <%s> by <%f> with spell <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpellEffect.ResourceName, SpellEffect.Value, *SpellData.Name);
		}
	}
	AffectedActors.AddUnique(TargetCharacter);
}

void AKSpell::ExecuteSpellEffects(TArray<FSpellEffect> SpellEffects)
{
	for (FSpellEffect SpellEffect : SpellEffects)
	{
		ExecuteSpellEffectByCollision(SpellEffect, CollisionComponent);
	}
	OnFinishExecuteSpellEffects_Native();
}

void AKSpell::ServerSpawnBuff_Implementation(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform)
{
	UKCombatFunctionLibrary::SpawnBuff(OwnerActor, TargetActor, BuffToSpawn, Transform);
}

void AKSpell::ExecuteBuffOnCharacter(TSubclassOf<AKBuff> Buff, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (!TargetCharacter || !OwnerCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("Unable to execute the Buff due to invalid Owner or Target."));
		return;
	}
	ServerSpawnBuff(OwnerCharacter, TargetCharacter, Buff, GetTransform());
}

void AKSpell::ExecuteBuffsOnCharacter(TArray<TSubclassOf<AKBuff>> Buffs, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (!TargetCharacter || !OwnerCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("Unable to execute the Buff due to invalid Owner or Target."));
		return;
	}
	for (AKBuff* Buff : Buffs) {
		ExecuteBuffOnCharacter(Buff, OwnerCharacter, TargetCharacter);
	}
}

void AKSpell::ServerResetAffectedActors_Implementation()
{
	if (HasAuthority())
		AffectedActors.Empty();
}

void AKSpell::OnFinishExecuteSpellEffects_Native()
{
	OnFinishExecuteSpellEffects();
}
