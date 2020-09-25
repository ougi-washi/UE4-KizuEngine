// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Combat/KAbility.h"

// Sets default values
AKAbility::AKAbility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AKAbility::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

