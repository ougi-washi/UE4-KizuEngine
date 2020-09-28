// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KBuff.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"

// Sets default values
AKBuff::AKBuff()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AKBuff::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate TargetActor.
	DOREPLIFETIME(AKBuff, OwnerActor);
	//Replicate TargetActor.
	DOREPLIFETIME(AKBuff, TargetActor);
}

// Called when the game starts or when spawned
void AKBuff::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		GetWorld()->GetTimerManager().SetTimer(TickingTimerHandle, this, &AKBuff::TriggerTicking, BuffData.TickingRate, true, BuffData.TickingDelay);
		GetWorld()->GetTimerManager().SetTimer(DurationHandle, this, &AKBuff::TriggerDurationEnd, BuffData.Duration, false, 0);
		if (BuffData.bAttachToTargetActor && TargetActor)
			MulticastAttachToTargetActor(BuffData.SocketName);
	}
}

// Called every frame
void AKBuff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKBuff::TriggerTicking()
{
	ExecuteAllBuffEffects();
}

void AKBuff::TriggerDurationEnd()
{
	AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor);
	if (BuffData.bUseTimeDilation && TargetCharacter) {
		TargetCharacter->ServerSetTimeDilation(1);
	}
	GetWorld()->GetTimerManager().ClearTimer(TickingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	Destroy(true, true);
}

void AKBuff::ServerSetTargetActor_Implementation(AActor* InTargetActor)
{
	TargetActor = InTargetActor;
}

void AKBuff::ServerSetOwnerActor_Implementation(AActor* InOwnerActor)
{
	OwnerActor = InOwnerActor;
	SetOwner(InOwnerActor)
}

void AKBuff::ExecuteAllBuffEffects()
{
	AKCharacter* OwnerCharacter = Cast<AKCharacter>(OwnerActor);
	AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor);

	if (!OwnerCharacter || !TargetCharacter)
		return;

	for (FBuffEffect& BuffEffect : BuffData.BuffEffects) {
		ExecuteBuffEffectOnCharacter(BuffEffect, OwnerCharacter, TargetCharacter);
	}
	
	if (BuffData.bUseTimeDilation) {
		TargetCharacter->ServerSetTimeDilation(BuffData.TimeDilationParams.Value);
	}
}

void AKBuff::ExecuteBuffEffectOnCharacter(FBuffEffect& BuffEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (BuffEffect.ResourceEffectType == EResourceEffectType::Consumption) {
		if (BuffEffect.bHealthResource) {
			OwnerCharacter->ServerApplyDamage(TargetCharacter, BuffEffect.Value, NULL);
			UE_LOG(LogKizu, Log, TEXT("<%s> damages <%s> by <%f> with debuff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, BuffEffect.Value, *BuffData.Name);
		}
		else {
			TargetCharacter->ConsumeResource(BuffEffect.ResourceName, BuffEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> drains <%s>'s <%s> by <%f> with debuff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *BuffEffect.ResourceName, BuffEffect.Value, *BuffData.Name);
		}
	}
	if (BuffEffect.ResourceEffectType == EResourceEffectType::Gain) {
		if (BuffEffect.bHealthResource) {
			TargetCharacter->GainHealth(BuffEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> heals <%s> by <%f> with buff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, BuffEffect.Value, *BuffData.Name);
		}
		else {
			TargetCharacter->GainResource(BuffEffect.ResourceName, BuffEffect.Value);
			UE_LOG(LogKizu, Log, TEXT("<%s> recovers <%s>'s <%s> by <%f> with buff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *BuffEffect.ResourceName, BuffEffect.Value, *BuffData.Name);
		}
	}
}

void AKBuff::ServerAttachToTargetActor_Implementation(const FName& SocketName)
{
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	if (TargetActor)
		AttachToActor(TargetActor, AttachmentTransformRules, SocketName);
}

void AKBuff::MulticastAttachToTargetActor_Implementation(const FName& SocketName)
{
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	if (TargetActor)
		AttachToActor(TargetActor, AttachmentTransformRules, SocketName);
}


