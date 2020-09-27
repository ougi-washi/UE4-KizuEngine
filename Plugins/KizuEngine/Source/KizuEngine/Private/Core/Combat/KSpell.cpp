// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Combat/KSpell.h"
#include "KizuEngine.h"
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
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKSpell::OnCollisionBeingOverlap);
	}
}

// Called when the game starts or when spawned
void AKSpell::BeginPlay()
{
	Super::BeginPlay();
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

void AKSpell::OnCollisionBeingOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ExecuteSpellEffects(GetAllOnHitEffects());
	OnCollisionBeingOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AKSpell::ExecuteSpellEffectByCollision(FSpellEffect SpellEffect, UPrimitiveComponent* inCollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	inCollisionComponent->GetOverlappingActors(OverlappingActors);
	int32 OwnerIndex = OverlappingActors.Find(inCollisionComponent->GetOwner()->GetOwner());
	UE_LOG(LogKizu, Warning, TEXT("Spell Owner : %s"), *inCollisionComponent->GetOwner()->GetOwner()->GetName());
	
	for (AActor* TempActor : OverlappingActors)
	{
		AKCharacter* OtherCharacter = Cast<AKCharacter>(TempActor);
		AKCharacter* OwnerCharacter = Cast<AKCharacter>(inCollisionComponent->GetOwner()->GetOwner());
		if (SpellEffect.bAffectOwner && TempActor == OwnerCharacter) {
			ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
		}
		if (SpellEffect.bAffectOtherFaction && OtherCharacter->CharacterData.Faction != OwnerCharacter->CharacterData.Faction) {
			ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
		}
		if (SpellEffect.bAffectOwnerFaction && OtherCharacter->CharacterData.Faction == OwnerCharacter->CharacterData.Faction)
		{
			ExecuteSpellEffectOnCharacter(SpellEffect, OwnerCharacter, OtherCharacter);
		}
	}		
}

void AKSpell::ExecuteSpellEffectOnCharacter(FSpellEffect SpellEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
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
}

void AKSpell::ExecuteSpellEffects(TArray<FSpellEffect> SpellEffects)
{
	for (FSpellEffect SpellEffect : SpellEffects)
	{
		ExecuteSpellEffectByCollision(SpellEffect, CollisionComponent);
	}
}
