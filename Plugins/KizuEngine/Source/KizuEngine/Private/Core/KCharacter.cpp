// Fill out your copyright notice in the Description page of Project Settings.

#include "KizuEngine.h"
#include "Engine/World.h"
#include "Core/KCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AKCharacter::AKCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCharacterMovement()->SetIsReplicated(true);

}

void AKCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate Character stats.
	DOREPLIFETIME(AKCharacter, CharacterData);
}

// Called when the game starts or when spawned
void AKCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKCharacter::ServerSetCharacterData_Implementation(const FCharacterData& inCharacterData)
{
	CharacterData = inCharacterData;
}

void AKCharacter::ServerSetCurrentHealth_Implementation(const float& inValue)
{
	CharacterData.CurrentHealth = inValue;

	OnCurrentHealthChange_Native();

	if (CharacterData.CurrentHealth <= 0.f) {
		ExecuteDeathEvent_Native();
	}
}

void AKCharacter::ServerSetCurrentResource_Implementation(const FString &ResourceName, const float& inValue)
{
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName)
			Resource.CurrentValue = inValue;
	}
}


bool AKCharacter::GetResource(const FString ResourceName, FResource& ResultResource)
{
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName) {
			ResultResource = Resource;
			return true;
		}
	}
	return false;
}

bool AKCharacter::GetResourceIndex(const FString ResourceName, int& ResourceIndex)
{
	int Index = 0;
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName) {
			ResourceIndex = Index;
			return true;
		}
		Index++;
	}
	return false;
}

bool AKCharacter::GetResourceCurrentValue(const FString ResourceName, float& ResultValue)
{
	FResource Resource;
	if (GetResource(ResourceName, Resource)) {
		ResultValue = Resource.CurrentValue;
		return true;
	}
	return false;
}



bool AKCharacter::GainResource(const FString ResourceName, const float ValueToGain)
{
	FResource Resource;
	if (GetResource(ResourceName, Resource)) {

		float FinalValue = Resource.CurrentValue + ValueToGain;
		if (FinalValue > Resource.MaxValue)
			FinalValue = Resource.MaxValue;
		ServerSetCurrentResource(ResourceName, FinalValue);

		OnResourceGain_Native(ResourceName, FinalValue);
		return true;
	}
	return false;
}

bool AKCharacter::ConsumeResource(const FString ResourceName, const float ValueToConsume)
{
	FResource Resource;
	if (GetResource(ResourceName, Resource)) {

		float FinalValue = Resource.CurrentValue - ValueToConsume;
		if (FinalValue < 0 && !Resource.bCanBeBelowZero)
			FinalValue = 0;
		ServerSetCurrentResource(ResourceName, FinalValue);

		OnResourceConsumption_Native(ResourceName, FinalValue);
		return true;
	}
	return false;
}

void AKCharacter::OnResourceGain_Native(const FString& ResourceName, const float& Value)
{
	OnResourceGain(ResourceName, Value);
}

void AKCharacter::OnResourceConsumption_Native(const FString& ResourceName, const float& Value)
{
	OnResourceConsumption(ResourceName, Value);
}

void AKCharacter::ServerApplyDamage_Implementation(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType)
{
	UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, DamageType);
}

bool AKCharacter::ServerApplyDamage_Validate(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType)
{
	return (Damage > 0);
}

float AKCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ServerSetCurrentHealth(CharacterData.CurrentHealth - Damage);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}



void AKCharacter::OnRep_CharacterData()
{
}

void AKCharacter::OnCurrentHealthChange_Native()
{

	OnCurrentHealthChange();
}

void AKCharacter::ExecuteDeathEvent_Native()
{
	if (bPlayDeathMontage && DeathMontage)
		MontagePlay_Replicated(DeathMontage);
	ExecuteDeathEvent();
}


// Called every frame
void AKCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AKCharacter::GetIsNetworked()
{
	return (!UKismetSystemLibrary::IsStandalone(this));
}

void AKCharacter::ServerSpawnActor_Implementation(UClass* Class, const FTransform& Transform)
{
	UWorld* World = GetWorld();
	if (World) {
		LastSpawnedActor = World->SpawnActor<AActor>(Class, Transform);
	}
}

void AKCharacter::MontagePlay_Replicated(UAnimMontage* Montage, const float Rate)
{
	if (Montage->IsValidLowLevel()) {
		ClientMontagePlay(Montage, Rate);
		if (GetIsNetworked())
			ServerMontagePlay(Montage, Rate);
	}
	else UE_LOG(LogKizu, Warning, TEXT("<MontagePlay_Replicated> Cannot play a NULL Montage."));
}

void AKCharacter::ServerMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	MulticastMontagePlay(Montage, Rate);
}

bool AKCharacter::ServerMontagePlay_Validate(UAnimMontage* Montage, const float Rate)
{
	return Montage->IsValidLowLevel();
}

void AKCharacter::ClientMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance->IsValidLowLevel()) {
		AnimInstance->Montage_Play(Montage, Rate);
	}
}

void AKCharacter::MulticastMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsLocallyControlled() && AnimInstance)
		AnimInstance->Montage_Play(Montage, Rate);
}

