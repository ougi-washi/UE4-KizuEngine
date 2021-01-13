// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KSpawnableAbility.h"
#include "KizuEngine.h"
#include "Core/Combat/KBuff.h"
#include "Net/UnrealNetwork.h"
#include "Core/KCharacter.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FunctionLibrary/KCombatFunctionLibrary.h"
#include "FunctionLibrary/KActionFunctionLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
AKSpawnableAbility::AKSpawnableAbility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	if (CollisionComponent) {
		SetRootComponent(CollisionComponent);
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKSpawnableAbility::OnCollisionBeginOverlap_Native);
	}
	// Adding a Movement component for the projectile
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	if (ProjectileMovementComponent) {
		ProjectileMovementComponent->SetIsReplicated(true);
		ProjectileMovementComponent->InitialSpeed = 1000.f;
		ProjectileMovementComponent->MaxSpeed = 1200.f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileMovementComponent->MaxSpeed * 10;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
	}
}

void AKSpawnableAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AKSpawnableAbility, AffectedActors);
}

// Called when the game starts or when spawned
void AKSpawnableAbility::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		ExecuteSpawnableAbilityEffects(GetAllOnSpawnEffects());
		if (SpawnableAbilityData.bTickEffects)
			GetWorld()->GetTimerManager().SetTimer(TickingTimerHandle, this, &AKSpawnableAbility::TriggerTicking, SpawnableAbilityData.TickingRate, true);
	}
}

// Called every frame
void AKSpawnableAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKSpawnableAbility::TriggerTicking()
{
	ServerResetAffectedActors();
	ExecuteSpawnableAbilityEffects(GetAllOnHitEffects());
}


void AKSpawnableAbility::InitializeMovement(const FVector InitialDirection, AActor* TargetActor)
{
	// Initialize the velocity
	ProjectileMovementComponent->Velocity = InitialDirection * ProjectileMovementComponent->InitialSpeed;
	// Check if homing and the target is available
	if (ProjectileMovementComponent->bIsHomingProjectile && TargetActor) {
		// look for a component in the center of the actor to travel towards it. The component should have "Center" as tag.
		TArray<UActorComponent*> TargetActorComponents = TargetActor->GetComponentsByTag(UActorComponent::StaticClass(), "Center");
		UActorComponent* TargetCenterActorComp;
		// Set the homing target component as the center component if found, else just pick the root component.
		if (TargetActorComponents.IsValidIndex(0)) {
			TargetCenterActorComp = TargetActorComponents[0];
			if (USceneComponent* TargetCenterSceneComp = Cast<USceneComponent>(TargetCenterActorComp))
				ProjectileMovementComponent->HomingTargetComponent = TargetCenterSceneComp;
		}
		else ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();
	}
}

void AKSpawnableAbility::ServerInitializeMovement_Implementation(const FVector InitialDirection, AActor* TargetActor)
{
	MulticastInitializeMovement(InitialDirection, TargetActor);
}

void AKSpawnableAbility::MulticastInitializeMovement_Implementation(const FVector InitialDirection, AActor* TargetActor)
{
	InitializeMovement(InitialDirection, TargetActor);
}

void AKSpawnableAbility::ServerSetOwner_Implementation(AActor* InOwnerActor)
{
	SetOwner(InOwnerActor);
}

TArray<FSpawnableAbilityEffect> AKSpawnableAbility::GetAllOnSpawnEffects()
{
	TArray<FSpawnableAbilityEffect> ResultEffects;
	for (FSpawnableAbilityEffect SpawnableAbilityEffect : SpawnableAbilityData.Effects)
	{
		if (SpawnableAbilityEffect.SpawnableAbilityTriggerType == ESpawnableAbilityTriggerType::OnSpawn)
			ResultEffects.Add(SpawnableAbilityEffect);
	}
	return ResultEffects;
}

TArray<FSpawnableAbilityEffect> AKSpawnableAbility::GetAllOnHitEffects()
{
	TArray<FSpawnableAbilityEffect> ResultEffects;
	for (FSpawnableAbilityEffect SpawnableAbilityEffect : SpawnableAbilityData.Effects)
	{
		if (SpawnableAbilityEffect.SpawnableAbilityTriggerType == ESpawnableAbilityTriggerType::OnHit)
			ResultEffects.Add(SpawnableAbilityEffect);
	}
	return ResultEffects;
}

bool AKSpawnableAbility::GetOverlappingActorsByTag(TArray<AActor*> &OverlappingActors, const FName PrimitiveComponentTag)
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

void AKSpawnableAbility::OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
		ExecuteSpawnableAbilityEffects(GetAllOnHitEffects());
	OnCollisionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AKSpawnableAbility::ExecuteSpawnableAbilityEffectByCollision(FSpawnableAbilityEffect &SpawnableAbilityEffect, UPrimitiveComponent* inCollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	inCollisionComponent->GetOverlappingActors(OverlappingActors);
	for (AActor* TempActor : OverlappingActors)
	{
		// Check if the SpawnableAbility is supposed to affect the overlapping actor only once. (This actor will be skipped in case it's not supposed to be affected again by this same effect)
		if (AffectedActors.Find(TempActor) > -1 && SpawnableAbilityData.bAffectOnce) {
			// Can't be affected
		}
		else {
			// Initiate effect on the actor
			AKCharacter* OtherCharacter = Cast<AKCharacter>(TempActor);
			AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());

			if (!OtherCharacter || !OwnerCharacter)
				return;

			if (SpawnableAbilityEffect.bAffectOwner && OtherCharacter == OwnerCharacter) {
				ExecuteSpawnableAbilityEffectOnCharacter(SpawnableAbilityEffect, OwnerCharacter, OtherCharacter);
				bHasHit = true;
			}
			if (SpawnableAbilityEffect.bAffectOtherFaction && OtherCharacter->CharacterData.Faction != OwnerCharacter->CharacterData.Faction) {
				ExecuteSpawnableAbilityEffectOnCharacter(SpawnableAbilityEffect, OwnerCharacter, OtherCharacter);
				bHasHit = true;
			}
			if ((SpawnableAbilityEffect.bAffectOwnerFaction && OtherCharacter->CharacterData.Faction == OwnerCharacter->CharacterData.Faction)
				&& (OtherCharacter != OwnerCharacter))
			{
				ExecuteSpawnableAbilityEffectOnCharacter(SpawnableAbilityEffect, OwnerCharacter, OtherCharacter);
				bHasHit = true;
			}
		}
	}		
}

void AKSpawnableAbility::ExecuteSpawnableAbilityEffectOnCharacter(FSpawnableAbilityEffect &SpawnableAbilityEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (SpawnableAbilityEffect.ResourceEffectType == EResourceEffectType::Consumption) {
		if (SpawnableAbilityEffect.bHealthResource) {
			OwnerCharacter->ServerApplyDamage(TargetCharacter, SpawnableAbilityEffect.Value, NULL);
			UE_LOG(LogKizu, Log, TEXT("<%s> damages <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, SpawnableAbilityEffect.Value, *SpawnableAbilityData.Name);
		}
		else {
			TargetCharacter->ConsumeResource(SpawnableAbilityEffect.ResourceName, SpawnableAbilityEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> drains <%s>'s <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpawnableAbilityEffect.ResourceName, SpawnableAbilityEffect.Value, *SpawnableAbilityData.Name);
		}
	}
	if (SpawnableAbilityEffect.ResourceEffectType == EResourceEffectType::Gain) {
		if (SpawnableAbilityEffect.bHealthResource) {
			TargetCharacter->GainHealth(SpawnableAbilityEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> heals <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, SpawnableAbilityEffect.Value, *SpawnableAbilityData.Name);
		}
		else {
			TargetCharacter->GainResource(SpawnableAbilityEffect.ResourceName, SpawnableAbilityEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> recovers <%s>'s <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpawnableAbilityEffect.ResourceName, SpawnableAbilityEffect.Value, *SpawnableAbilityData.Name);
		}
	}

	UKActionFunctionLibrary::SendReaction(OwnerCharacter, TargetCharacter, SpawnableAbilityEffect.ReactionSendingData);
	
	ExecuteBuffsOnCharacter(SpawnableAbilityEffect.Buffs, OwnerCharacter, TargetCharacter);
	AffectedActors.AddUnique(TargetCharacter);
}

void AKSpawnableAbility::ExecuteSpawnableAbilityEffects(TArray<FSpawnableAbilityEffect> SpawnableAbilityEffects)
{
	for (FSpawnableAbilityEffect SpawnableAbilityEffect : SpawnableAbilityEffects)
	{
		ExecuteSpawnableAbilityEffectByCollision(SpawnableAbilityEffect, CollisionComponent);
	}
	// Call the Finish execute event
	OnFinishExecuteSpawnableAbilityEffects_Native();
}

void AKSpawnableAbility::ServerSpawnBuff_Implementation(AActor* OwnerActor, AActor* TargetActor, TSubclassOf<AKBuff> BuffToSpawn, const FTransform Transform)
{
	UKCombatFunctionLibrary::SpawnBuff(OwnerActor, TargetActor, BuffToSpawn, Transform);
}

void AKSpawnableAbility::ExecuteBuffOnCharacter(TSubclassOf<AKBuff> Buff, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (!TargetCharacter || !OwnerCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("Unable to execute the Buff due to invalid Owner or Target."));
		return;
	}
	ServerSpawnBuff(OwnerCharacter, TargetCharacter, Buff, GetTransform());
}

void AKSpawnableAbility::ExecuteBuffsOnCharacter(TArray<TSubclassOf<AKBuff>> Buffs, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (!TargetCharacter || !OwnerCharacter) {
		UE_LOG(LogKizu, Warning, TEXT("Unable to execute the Buff due to invalid Owner or Target."));
		return;
	}
	for (TSubclassOf<AKBuff> Buff : Buffs) {
		ExecuteBuffOnCharacter(Buff, OwnerCharacter, TargetCharacter);
	}
}

void AKSpawnableAbility::ServerResetAffectedActors_Implementation()
{
	if (HasAuthority())
		AffectedActors.Empty();
}

void AKSpawnableAbility::OnFinishExecuteSpawnableAbilityEffects_Native()
{
	if (SpawnableAbilityData.bDestroyOnHit && bHasHit) {
		TriggerDestroytimer(SpawnableAbilityData.DestroyTimer);
	}
	OnFinishExecuteSpawnableAbilityEffects();
}

void AKSpawnableAbility::TriggerDestroytimer(const float DestroyTimer)
{
	// Destroy(true); // Using Destroy will not wait for the projectile effects to function properly, for that we hide it and give it a second until everything has properly functioned for all the clients in the ApplyProjectileEffects
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(UKismetMathLibrary::Abs(DestroyTimer)); // 2 seconds on default, it can vary depending on how much of a net latency is at the client however a game with 2000ms is not really playable.
}
