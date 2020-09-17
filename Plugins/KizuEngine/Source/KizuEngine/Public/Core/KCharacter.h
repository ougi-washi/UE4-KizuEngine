// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "KCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData 
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		float MaxHealth = 100;

	/** The player's current health. When reduced to 0, they are considered dead.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
		float CurrentHealth = 100;


};

UCLASS()
class KIZUENGINE_API AKCharacter : public ACharacter
{
	GENERATED_BODY()
		
public:
	// Sets default values for this character's properties
	AKCharacter();
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_CharacterData, Category = "Kizu|Character|Data")
		FCharacterData CharacterData;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
		void OnRep_CharacterData();
	/** Response to Data being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnCharacterDataUpdate();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Returns Checks if the player is networked
	 * @return True if the player is networked, false if it's a standalone object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Network")
		uint8 GetIsNetworked();

	/**
	 * Play Montage on server and all clients (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Animation")
		void MontagePlay_Replicated(UAnimMontage* Montage, const float Rate = 1.f);
	/**
	 * Play Montage on server and all clients (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Kizu|Character|Animation")
		void ServerMontagePlay(UAnimMontage* Montage, const float Rate = 1.f);
	/**
	 * Play Montage on client (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Kizu|Character|Animation")
		void ClientMontagePlay(UAnimMontage* Montage, const float Rate = 1.f);
	/**
	 * Play Montage on all clients (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Character|Animation")
		void MulticastMontagePlay(UAnimMontage* Montage, const float Rate = 1.f);


};
