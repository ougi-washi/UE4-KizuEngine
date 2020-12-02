// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/KItem.h"

// Sets default values
AKItem::AKItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

