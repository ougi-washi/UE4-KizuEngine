// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/KAction.h"
#include "KizuEngine.h"
#include "Core/KCharacter.h"

// Sets default values
AKAction::AKAction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AKAction::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

