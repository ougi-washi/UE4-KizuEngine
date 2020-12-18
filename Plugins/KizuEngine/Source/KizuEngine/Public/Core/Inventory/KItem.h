// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "KItem.generated.h"


USTRUCT(BlueprintType)
struct FItemEffect {
	GENERATED_USTRUCT_BODY()

	/** The resource name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	FString Name = "None";
	/** The value to affect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Item")
	float Value = 100.f;
};

USTRUCT(BlueprintType)
struct FItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FItemEffect> UsageEffects;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bUseStaticMesh : 1 ;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUseStaticMesh"))
	UStaticMesh* StaticMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bUseStaticMesh"))
	USkeletalMesh* SkeletalMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	float Weight = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	int Price = 1;

	FItem() : FTableRowBase() {
		bUseStaticMesh = 1;
	}
};
// FItem == operator 
inline bool operator==(const FItem& lhs, const FItem& rhs)
{
	return lhs.Id == rhs.Id;
}

UCLASS()
class KIZUENGINE_API AKItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKItem();

	/** Item data related to it, it represents its name, mesh and other. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItem ItemData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
