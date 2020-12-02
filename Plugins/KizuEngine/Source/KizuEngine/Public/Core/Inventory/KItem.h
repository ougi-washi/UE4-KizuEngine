// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "KItem.generated.h"


USTRUCT(BlueprintType)
struct FItemEffect {
	GENERATED_USTRUCT_BODY()

	/** The custom resource name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	FString Name = "None";
	/** The custom resource max value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	float MaxValue = 100.f;
};

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//FItem
};

UCLASS()
class KIZUENGINE_API AKItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKItem();

	FItem ItemData;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
