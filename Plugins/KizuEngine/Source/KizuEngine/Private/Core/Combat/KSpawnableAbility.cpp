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
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"

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
		ProjectileMovementComponent->bIsHomingProjectile = false;
		ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileMovementComponent->MaxSpeed * 10;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->HomingTargetComponent = nullptr;
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
	if (HasAuthority())
	{
		ExecuteSpawnableAbilityEffects(GetAllOnSpawnEffects());
		if (SpawnableAbilityData.bTickEffects)
			GetWorld()->GetTimerManager().SetTimer(TickingTimerHandle, this, &AKSpawnableAbility::TriggerTicking, SpawnableAbilityData.TickingRate, true);
	}

	if (SpawnableAbilityData.ParticleSystemOnSpawn)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, SpawnableAbilityData.ParticleSystemOnSpawn, GetActorLocation(), GetActorRotation(), SpawnableAbilityData.ScaleParticleOnSpawn, true);
	}

	if (SpawnableAbilityData.bSnapToGround)
	{
		SnapToGround();
	}

	if (SpawnableAbilityData.bAttachToOwner)
	{
		// Try attaching to the owner character mesh, if it fails attach to the owner's root component.
		if (!AttachToOwnerCharacterMesh(SpawnableAbilityData.AttachSocketName))
		{
			AttachToOwner();
		}
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
	if (ProjectileMovementComponent->bIsHomingProjectile && TargetActor) 
	{
		if (CanHomeToTarget(TargetActor))
		{
			// look for a component in the center of the actor to travel towards it. The component should have "Center" as tag.
			TArray<UActorComponent*> TargetActorComponents = TargetActor->GetComponentsByTag(UActorComponent::StaticClass(), "Center");
			UActorComponent* TargetCenterActorComp;
			// Set the homing target component as the center component if found, else just pick the root component.
			if (TargetActorComponents.IsValidIndex(0))
			{
				TargetCenterActorComp = TargetActorComponents[0];
				if (USceneComponent* TargetCenterSceneComp = Cast<USceneComponent>(TargetCenterActorComp))
				{
					ProjectileMovementComponent->HomingTargetComponent = TargetCenterSceneComp;
				}
			}
			else 
			{
				ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();
			}
		}
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
		{
			ResultEffects.Add(SpawnableAbilityEffect);
		}
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
				if (TempActor != this)
				{
					OverlappingActors.AddUnique(TempActor);
				}
			}
		}

	if (OverlappingActors.Num() > 0)
		return true;
	return false;
}

void AKSpawnableAbility::OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ExecuteSpawnableAbilityEffects(GetAllOnHitEffects());	
	}
	OnCollisionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AKSpawnableAbility::Multicast_SetActorEnableCollision_Implementation(bool bNewValue)
{
	SetActorEnableCollision(bNewValue);
}

bool AKSpawnableAbility::CanHomeToTarget(AActor* TargetActor)
{
	AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());
	if (AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor))
	{
		bool bSameFaction = UKCombatFunctionLibrary::IsSameFaction(OwnerCharacter, TargetCharacter);

		if (SpawnableAbilityData.Effects.IsValidIndex(0))
		{
			if (SpawnableAbilityData.Effects[0].bAffectOtherFaction && !bSameFaction)
			{
				return true;
			}
			else if (SpawnableAbilityData.Effects[0].bAffectOwnerFaction && bSameFaction)
			{
				return true;
			}
		}
	}
	return false;
}

void AKSpawnableAbility::SnapToGround()
{
	FHitResult Outhit;
	FCollisionQueryParams Params;
	FCollisionResponseParams CollisionResponseParam;
	Params.AddIgnoredActor(this);

	if (UWorld* World = GetWorld())
	{
		if (World->LineTraceSingleByChannel(Outhit,
			GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 500),
			ECollisionChannel::ECC_Visibility, Params, CollisionResponseParam))
		{
			SetActorLocation(Outhit.ImpactPoint);
		}
	}
}

bool AKSpawnableAbility::AttachToOwnerCharacterMesh(const FName SocketName)
{
	if (AKCharacter* KCharacter = Cast<AKCharacter> (GetOwner()))
	{
		if (KCharacter->GetMesh())
		{
			AttachToComponent(KCharacter->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, SocketName);
			return true;
		}
	}
	return false;
}

bool AKSpawnableAbility::AttachToOwner()
{
	if (GetOwner())
	{
		AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		return true;
	}
	return false;
}


float AKSpawnableAbility::GetEffectValue(FSpawnableAbilityEffect& SpawnableAbilityEffect)
{
	if (SpawnableAbilityEffect.EffectValueType == ESpawnableAbilityDamageValueType::SADV_Static)
	{
		return SpawnableAbilityEffect.Value;
	}
	else if (SpawnableAbilityEffect.EffectValueType == ESpawnableAbilityDamageValueType::SADV_DynamicOnSource)
	{
		if (AKCharacter* KCharacter = Cast<AKCharacter>(GetOwner()))
		{
			float ResultValue;
			if (KCharacter->GetResourceCurrentValue(SpawnableAbilityEffect.SourceResourceName, ResultValue))
			{
				ResultValue = (ResultValue * SpawnableAbilityEffect.Value) / 100;
				float MinValue;
				float MaxValue;
				UKCombatFunctionLibrary::GetVariance(MinValue, MaxValue, ResultValue);
				ResultValue = UKismetMathLibrary::RandomIntegerInRange(MinValue, MaxValue);
				return ResultValue;
			}
		}
	}
	UE_LOG(LogKizu, Warning, TEXT("Issue occured when calculation the effect value of %s"), *GetName());
	return 0.f;
}

void AKSpawnableAbility::ExecuteSpawnableAbilityEffectByCollision(FSpawnableAbilityEffect &SpawnableAbilityEffect, UPrimitiveComponent* inCollisionComponent)
{
	TArray<AActor*> OverlappingActors;
	inCollisionComponent->GetOverlappingActors(OverlappingActors);
	for (AActor* TempActor : OverlappingActors)
	{
		// Check if the SpawnableAbility is supposed to affect the overlapping actor only once. (This actor will be skipped in case it's not supposed to be affected again by this same effect)
		if (AffectedActors.Find(TempActor) > -1 && SpawnableAbilityData.bAffectOnce) 
		{
			// Can't be affected
		}
		else {
			// Initiate effect on the actor
			AKCharacter* OtherCharacter = Cast<AKCharacter>(TempActor);
			AKCharacter* OwnerCharacter = Cast<AKCharacter>(GetOwner());

			if (!OtherCharacter || !OwnerCharacter)
				return;

			if (SpawnableAbilityEffect.bAffectOwner && OtherCharacter == OwnerCharacter) 
			{
				ExecuteSpawnableAbilityEffectOnCharacter(SpawnableAbilityEffect, OwnerCharacter, OtherCharacter);
				bHasHit = true;
			}
			if (SpawnableAbilityEffect.bAffectOtherFaction && OtherCharacter->CharacterData.Faction != OwnerCharacter->CharacterData.Faction) 
			{
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
	float EffectValue  = GetEffectValue(SpawnableAbilityEffect);
	if (SpawnableAbilityEffect.ResourceEffectType == EResourceEffectType::Consumption) 
	{
		if (SpawnableAbilityEffect.bHealthResource) 
		{
			OwnerCharacter->ServerApplyDamage(TargetCharacter, EffectValue, SpawnableAbilityEffect.DamageType);
			UE_LOG(LogKizu, Log, TEXT("<%s> damages <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, EffectValue, *SpawnableAbilityData.Name);
		}
		else 
		{
			TargetCharacter->ConsumeResource(SpawnableAbilityEffect.ResourceName, EffectValue);
			UE_LOG(LogKizu, Log, TEXT("<%s> drains <%s>'s <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpawnableAbilityEffect.ResourceName, EffectValue, *SpawnableAbilityData.Name);
		}
	}
	if (SpawnableAbilityEffect.ResourceEffectType == EResourceEffectType::Gain) 
	{
		if (SpawnableAbilityEffect.bHealthResource) 
		{
			TargetCharacter->GainHealth(EffectValue);
			UE_LOG(LogKizu, Log, TEXT("<%s> heals <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, EffectValue, *SpawnableAbilityData.Name);
		}
		else 
		{
			TargetCharacter->GainResource(SpawnableAbilityEffect.ResourceName, EffectValue);
			UE_LOG(LogKizu, Log, TEXT("<%s> recovers <%s>'s <%s> by <%f> with SpawnableAbility <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *SpawnableAbilityEffect.ResourceName, EffectValue, *SpawnableAbilityData.Name);
		}
	}

	// Send reaction to the affected character
	UKActionFunctionLibrary::SendReaction(OwnerCharacter, TargetCharacter, SpawnableAbilityEffect.ReactionSendingData);
	
	// Applying a buff to the affected character
	ExecuteBuffsOnCharacter(SpawnableAbilityEffect.Buffs, OwnerCharacter, TargetCharacter);
	
	// Spawning a particle on effect application
	if( SpawnableAbilityEffect.bSpawnEmitterOnApply && SpawnableAbilityEffect.ParticleSystem)
	{
		MulticastSpawnEmitter(SpawnableAbilityEffect.ParticleSystem);
	}

	OnApplyEffectToCharacter_Native(TargetCharacter);

	// Add to the affected actors
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
	{
		AffectedActors.Empty();
	}	
}

void AKSpawnableAbility::OnFinishExecuteSpawnableAbilityEffects_Native()
{
	if (SpawnableAbilityData.bDestroyOnHit && bHasHit) {
		TriggerDestroyTimer(SpawnableAbilityData.DestroyTimer);
	}
	OnFinishExecuteSpawnableAbilityEffects();
}

void AKSpawnableAbility::TriggerDestroyTimer(const float DestroyTimer)
{
	// Destroy(true); // Using Destroy will not wait for the projectile effects to function properly, for that we hide it and give it a second until everything has properly functioned for all the clients in the ApplyProjectileEffects
	SetActorHiddenInGame(true);
	Multicast_SetActorEnableCollision(false);
	OnStartDestruction_Native();
	SetLifeSpan(UKismetMathLibrary::Abs(DestroyTimer)); // 2 seconds on default, it can vary depending on how much of a net latency is at the client however a game with 2000ms is not really playable.
}

void AKSpawnableAbility::OnStartDestruction_Native()
{
	if (SpawnableAbilityData.bSpawnEmitterOnDestroy)
	{
		MulticastSpawnEmitter_Implementation(SpawnableAbilityData.ParticleSystemOnDestroy);
	}
	OnStartDestruction();
}

void AKSpawnableAbility::OnApplyEffectToCharacter_Native(AKCharacter* TargetCharter)
{
	OnApplyEffectToCharacter(TargetCharter);
}

void AKSpawnableAbility::ServerSpawnEmitter_Implementation(UParticleSystem* EmitterTemplate)
{
	MulticastSpawnEmitter_Implementation(EmitterTemplate);
}

void AKSpawnableAbility::MulticastSpawnEmitter_Implementation(UParticleSystem* EmitterTemplate)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, EmitterTemplate, GetActorLocation(), GetActorRotation(), true);
}