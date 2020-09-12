// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/KCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AKCharacter::AKCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCharacterMovement()->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void AKCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

void AKCharacter::ServerPlayMontage_Implementation(const UAnimMontage* Montage, const float Rate)
{
	
}

bool AKCharacter::ServerPlayMontage_Validate(const UAnimMontage* Montage, const float Rate)
{
	return Montage->IsValidLowLevel();
}

