// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Combat/KSpell.h"
#include "KizuEngine.h"
#include "Net/UnrealNetwork.h"
#include "Core/KCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AKSpell::AKSpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	if (CollisionComponent) {
		SetRootComponent(CollisionComponent);
		CollisionComponent->bHiddenInGame = false;
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKSpell::OnCollisionBeginOverlap_Native);
	}
}

void AKSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//DOREPLIFETIME(AKSpell, AffectedActors);
}

// Called when the game starts or when spawned
void AKSpell::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
		ExecuteSpellEffects(GetAllOnSpawnEffects());
}

// Called every frame
void AKSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		// Check if the spell is supposed to affect the overlapping actor only once. (This actor will be skipped incase it's not supposed to be affected again by this same effect)
		if (AffectedActors.Find(TempActor) > -1 && SpellData.bAffectOnce) {
			// Can't be affected
		}
		else {
			// Initiate effect on the actor
			AKCharacter* OtherCharacter = Cast<AKCharacter>(TempActor);
			AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());

			if (!OtherCharacter || !OwnerCharacter)
				return;

			if (SpellEffect.bAffectOwner && TempActor == OwnerCharacter) {
				UE_LOG(LogKizu, Warning, TEXT("Target is Owner"));
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
			}
			if (SpellEffect.bAffectOtherFaction && OtherCharacter->CharacterData.Faction != OwnerCharacter->CharacterData.Faction) {
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
				UE_LOG(LogKizu, Warning, TEXT("Target is from another faction"));
			}
			if (SpellEffect.bAffectOwnerFaction && OtherCharacter->CharacterData.Faction == OwnerCharacter->CharacterData.Faction)
			{
				ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
				UE_LOG(LogKizu, Warning, TEXT("Target is from same faction"));
			}
		}
	}		
}

void AKSpell::ExecuteSpellEffectOnCharacter(FSpellEffect &SpellEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (SpellEffect.ResourceEffectType == EResourceEffectType::Consumption) {
		if (SpellEffect.bHealthResource) {
			OwnerCharacter->ServerApplyDamage(TargetCharacter, SpellEffect.Value, NULL);
		}
		else {
			TargetCharacter->ConsumeResource(SpellEffect.ResourceName, SpellEffect.Value);
		}
	}
	if (SpellEffect.ResourceEffectType == EResourceEffectType::Gain) {
		if (SpellEffect.bHealthResource) {
			//OwnerCharacter->ServerApplyDamage(TargetCharacter, SpellEffect.Value, NULL); To implement healing here
		}
		else {
			TargetCharacter->GainResource(SpellEffect.ResourceName, SpellEffect.Value);
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

void AKSpell::ServerResetAffectedActors_Implementation()
{
	if (HasAuthority())
		AffectedActors.Empty();
}

void AKSpell::OnFinishExecuteSpellEffects_Native()
{
	OnFinishExecuteSpellEffects();
}
