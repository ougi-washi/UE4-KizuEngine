// Fill out your copyright notice in the Description page of Project Settings.

#include "KizuEngine.h"
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

	//Replicate current health.
	DOREPLIFETIME(AKCharacter, CharacterData);
}

// Called when the game starts or when spawned
void AKCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKCharacter::OnRep_CharacterData()
{
	OnCharacterDataUpdate();
}

void AKCharacter::OnCharacterDataUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CharacterData.CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CharacterData.CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CharacterData.CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
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

uint8 AKCharacter::GetIsNetworked()
{
	return (!UKismetSystemLibrary::IsStandalone(this));
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

