// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/Combat/KBuff.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "FunctionLibrary/KCombatFunctionLibrary.h"

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
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TickingTimerHandle, this, &AKBuff::TriggerTicking, BuffData.TickingRate, true, BuffData.TickingDelay);
		GetWorld()->GetTimerManager().SetTimer(DurationHandle, this, &AKBuff::TriggerDurationEnd, .1f, true, BuffData.Duration);
		if (BuffData.bAttachToTargetActor && TargetActor)
		{
			MulticastAttachToTargetActor(BuffData.SocketName);
		}
		if (AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor))
		{
			TargetCharacter->Server_AddStatusEffect(this);
		}
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
	OnBuffTick();
}

void AKBuff::TriggerDurationEnd()
{
	AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor);
	if (TargetCharacter) 
	{
		TargetCharacter->RemoveStatusEffect(this);
		if (BuffData.bUseTimeDilation)
		{
			TargetCharacter->ServerSetTimeDilation(1);
		}
	}
	OnBuffEnd();

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
	SetOwner(InOwnerActor);
}

void AKBuff::ExecuteAllBuffEffects()
{
	AKCharacter* OwnerCharacter = Cast<AKCharacter>(OwnerActor);
	AKCharacter* TargetCharacter = Cast<AKCharacter>(TargetActor);

	if (!OwnerCharacter || !TargetCharacter)
		return;

	for (FBuffEffect& BuffEffect : BuffData.BuffEffects) 
	{
		ExecuteBuffEffectOnCharacter(BuffEffect, OwnerCharacter, TargetCharacter);
	}
	
	if (BuffData.bUseTimeDilation) 
	{
		TargetCharacter->ServerSetTimeDilation(BuffData.TimeDilationParams.Value);
	}

	if (BuffData.bUseMovementSpeedModifier)
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed += BuffData.MovementSpeedModifier;
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched += BuffData.MovementSpeedModifier;
		TargetCharacter->GetCharacterMovement()->MaxSwimSpeed += BuffData.MovementSpeedModifier;
	}
}

void AKBuff::ExecuteBuffEffectOnCharacter(FBuffEffect& BuffEffect, AKCharacter* OwnerCharacter, AKCharacter* TargetCharacter)
{
	if (BuffEffect.ResourceEffectType == EResourceEffectType::Consumption) {
		if (BuffEffect.bHealthResource) {
			OwnerCharacter->ServerApplyDamage(TargetCharacter, GetEffectValue(BuffEffect), NULL);
			UE_LOG(LogKizu, Log, TEXT("<%s> damages <%s> by <%f> with debuff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, BuffEffect.Value, *BuffData.Name);
		}
		else {
			TargetCharacter->ConsumeResource(BuffEffect.ResourceName, GetEffectValue(BuffEffect));
			UE_LOG(LogKizu, Log, TEXT("<%s> drains <%s>'s <%s> by <%f> with debuff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *BuffEffect.ResourceName, BuffEffect.Value, *BuffData.Name);
		}
	}
	if (BuffEffect.ResourceEffectType == EResourceEffectType::Gain) {
		if (BuffEffect.bHealthResource) {
			TargetCharacter->GainHealth(GetEffectValue(BuffEffect));
			UE_LOG(LogKizu, Log, TEXT("<%s> heals <%s> by <%f> with buff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, BuffEffect.Value, *BuffData.Name);
		}
		else {
			TargetCharacter->GainResource(BuffEffect.ResourceName, GetEffectValue(BuffEffect));
			UE_LOG(LogKizu, Log, TEXT("<%s> recovers <%s>'s <%s> by <%f> with buff <%s>"), *OwnerCharacter->CharacterData.Name, *TargetCharacter->CharacterData.Name, *BuffEffect.ResourceName, BuffEffect.Value, *BuffData.Name);
		}
	}
	OnExecuteBuff_Native(TargetCharacter, BuffEffect);
}


void AKBuff::OnExecuteBuff_Native(AKCharacter* AffectedCharacter, FBuffEffect BuffEffect)
{
	OnExecuteBuff(AffectedCharacter, BuffEffect);
}

void AKBuff::ServerAttachToTargetActor_Implementation(const FName& SocketName)
{
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	if (TargetActor)
	{
		if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
		{
			AttachToComponent(TargetCharacter->GetMesh(), AttachmentTransformRules, SocketName);
		}
		else
		{
			AttachToActor(TargetActor, AttachmentTransformRules, SocketName);
		}
	}
}

void AKBuff::MulticastAttachToTargetActor_Implementation(const FName& SocketName)
{
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	if (TargetActor)
	{
		if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
		{
			AttachToComponent(TargetCharacter->GetMesh(), AttachmentTransformRules, SocketName);
		}
		else
		{
			AttachToActor(TargetActor, AttachmentTransformRules, SocketName);
		}
	}
}

float AKBuff::GetEffectValue(const FBuffEffect& BuffEffect)
{
	if (BuffEffect.EffectValueType == EBuffDamageValueType::BDV_Static)
	{
		return BuffEffect.Value;
	}
	else if (BuffEffect.EffectValueType == EBuffDamageValueType::BDV_DynamicOnSource)
	{
		if (AKCharacter* KCharacter = Cast<AKCharacter>(GetOwner()))
		{
			float ResultValue;
			if (KCharacter->GetResourceCurrentValue(BuffEffect.SourceResourceName, ResultValue))
			{
				ResultValue = (ResultValue * BuffEffect.Value) / 100;
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