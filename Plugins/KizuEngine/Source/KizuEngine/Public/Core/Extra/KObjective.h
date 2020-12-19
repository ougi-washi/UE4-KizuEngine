// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"
#include "KObjective.generated.h"

/**
 * 
 */

 class ULevelSequence;

USTRUCT(BlueprintType)
struct FObjectiveRequirement
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Description = "In here is the description of the task/requirement";
};


USTRUCT(BlueprintType)
struct FObjective
{
	GENERATED_USTRUCT_BODY()

public: 
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Id = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = "None";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FObjectiveRequirement> Requirements;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ULevelSequence* LevelSequenceToPlay;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Description = "In here is the description of the Objective";
	
};


UCLASS()
class KIZUENGINE_API UKObjective : public UObject
{
	GENERATED_BODY()
	
};
