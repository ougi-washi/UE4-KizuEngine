// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Core/KAction.h"
#include "KCharacter.generated.h"


USTRUCT(BlueprintType)
struct FResource {

	GENERATED_USTRUCT_BODY()

public:
	/** The custom resource name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "None";
	/** The custom resource max value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxValue = 100.f;
	/** The custom resource current value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentValue = 100.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	bool bCanBeBelowZero = false;
};

USTRUCT(BlueprintType)
struct FCharacterData 
{
	GENERATED_USTRUCT_BODY()

public:
	/** The character name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "None";
	/** The character max health. The current health can never exceed this amount */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxHealth = 100.f;
	/** The character current health. When reduced to 0, they are considered dead. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentHealth = 100.f;
	/** Custom resources array (Examples: Energy, Mana, Armors..) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FResource> Resources;
	/** Faction to define either it's an enemy or an ally to another Faction */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	uint8 Faction = 0;
};

USTRUCT(BlueprintType)
struct FCustomDamage 
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	FString ID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	float Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Damage")
	TSubclassOf<UDamageType> DamageType;

	FORCEINLINE bool operator==(const FCustomDamage& rhs) const
	{
		return (rhs.ID == this->ID);
	}

	FCustomDamage() {
		// Default Constructor
	}

	FCustomDamage(FString InID) {
		this->ID = InID;
	}
};

UCLASS()
class KIZUENGINE_API AKCharacter : public ACharacter
{
	GENERATED_BODY()
		
private:

	bool AddCooldownToStack(const FCooldown& Cooldown);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Temp pointer to the last spawned Actor.
	UPROPERTY(Replicated)
	AActor* LastSpawnedActorRef;
	// Custom Damage to fill, clear and use during Gameplay.
	UPROPERTY()
	TArray<FCustomDamage> CustomDamageStack;

public:	

	/** The character's Data, containing all kind of general stats that vary during the Gameplay.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_CharacterData, Category = "Kizu|Character|Data")
	FCharacterData CharacterData;
	/** If the character plays the death montage on death */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data|Death")
	bool bPlayDeathMontage = false;
	/** This death montage is played on the death of the character (Current health equals 0)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bPlayDeathMontage"), Category = "Kizu|Character|Data|Death")
	UAnimMontage* DeathMontage;
	/** The Cooldown stack that holds are the Cooldowns */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FCooldown> CooldownStack;

	// Sets default values for this character's properties
	AKCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * General functionalities
	 */

	 /**
	  * Returns Checks if the player is networked
	  * @return True if the player is networked, false if it's a standalone object
	  */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Network")
	bool GetIsNetworked();
	/*
	* Replicated Actor Spawn
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|General")
	void ServerSpawnActor(UClass* Class, const FTransform& Transform);

	/**
	 * Character Data functionalities
	 */

	 /** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnRep_CharacterData();
	/** Response to Data being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
	void OnCurrentHealthChange();
	virtual void OnCurrentHealthChange_Native();
	/** Execute the death event (Animation/Events/etc...) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
	void ExecuteDeathEvent();
	virtual void ExecuteDeathEvent_Native();
	/** Sets the character data during the initialization or to use when changing multiple variables.*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Data")
	void ServerSetCharacterData(const FCharacterData& inCharacterData);
	/** Sets the character current health.*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Data")
	void ServerSetCurrentHealth(const float inValue);
	/** Sets the character current energy.*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Data")
	void ServerSetCurrentResource(const FString &ResourceName, const float inValue);
	/** Sets the character Faction.*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Data")
	void ServerSetFaction(const uint8 NewFaction);
	/**
	* Gain health on the server by checking the possible maximum.
	* @param ValueToGain The value to add to the character health
	*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Data")
	bool GainHealth(const float ValueToGain = 10);
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	/**
	* Get a resource from the character data
	* @param ResourceName The resource name to look for in the Array
	* @param ResultResource The resulting resource
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool GetResource(const FString ResourceName, FResource& ResultResource);
	/**
	* Get a resource index from the character data
	* @param ResourceName The resource name to look for in the Array
	* @param ResourceIndex The resulting resource index
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool GetResourceIndex(const FString ResourceName, int& ResourceIndex);
	/**
	* Get a resource current value from the character data
	* @param ResourceName The resource name to look for in the Array
	* @param ResultValue The resulting current value of the resource found
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool GetResourceCurrentValue(const FString ResourceName, float& ResultValue);
	/**
	* Gain resource on the server by checking the possible maximum.
	* @param ResourceName The resource name to look for in the Array
	* @param ValueToGain The value to add to the resource given
	*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Data")
	bool GainResource(const FString ResourceName, const float ValueToGain = 10);
	/**
	* Consume resource on the server by checking the possibility of being below 0 (Check with the boolean while setting up this resource).
	* @param ResourceName The resource name to look for in the Array
	* @param ValueToConsume The value to subtract from the resource given
	* @param bCheckEnoughResource Checks if there are enough resources
	*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Data")
	bool ConsumeResource(const FString ResourceName, const float ValueToConsume = 10.f, const bool bCheckEnoughResource = true);
	/**
	* Event called when gaining a resource
	* @param ResourceName The resource name
	* @param Value The value gained
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
	void OnResourceGain(const FString& ResourceName, const float& Value);
	virtual void OnResourceGain_Native(const FString& ResourceName, const float& Value);
	/**
	* Event called when consuming a resource
	* @param ResourceName The resource name
	* @param Value The value consumed
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
		void OnResourceConsumption(const FString& ResourceName, const float& Value);
	virtual void OnResourceConsumption_Native(const FString& ResourceName, const float& Value);
	/**
	* Event called when gaining health
	* @param Value The value gained
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
	void OnHealthGain(const float& Value);
	virtual void OnHealthGain_Native(const float& Value);
	/**
	* Event called when losing health
	* @param Value The value lost
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Data")
	void OnHealthLoss(const float& Value);
	virtual void OnHealthLoss_Native(const float& Value);

	/**
	 * Character Combat functionalities
	 */

	/** Apply damage to an Actor (replicated).*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Kizu|Character|Combat")
	void ServerApplyDamage(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType);
	/** Checks if the character has enough health. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool HasEnoughHealth(const float Value = 50.f);
	/** Checks if the character has enough from the resource given in the parameters. Returns false if the resource was not found.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool HasEnoughResource(const FString ResourceName, const float Value = 50.f);
	/** Adds a custom damage to the character (use "Make Custom Damage" in Blueprints before adding it)*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Combat")
	bool AddCustomDamage(const FCustomDamage& CustomDamage);
	/** Get a custom damage from the custom damage stack */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Combat")
	bool GetCustomDamage(const FString InID, FCustomDamage& OutCustomDamage);
	/** Edit the custom damage in the Custom Damage stack */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Combat")
	bool EditCustomDamage(const FString InID, const FCustomDamage& InCustomDamage);

	/**
	 * Character Montage and Animation Functionalities
	 */

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
	/**
	 * Set time dilation on the server (Replicated)
	 * @param TimeDilation new input time dilation to apply
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Kizu|Buff|Effect")
	void ServerSetTimeDilation(const float TimeDilation);
	/**
	 * Set time dilation on the clients
	 * @param TimeDilation new input time dilation to apply
	 */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Kizu|Buff|Effect")
	void MulticastSetTimeDilation(const float TimeDilation);

	/**
	 * Actions and Cooldowns
	 */

	/**
	 * Executes a given action while choosing if it should be added to the Cooldown stack or not (Replicated)
	 * @param ActionData The action to execute in this function
	 * @param bUseCooldown If a Cooldown will be added to the stacks
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Action")
	bool ExecuteAction(const FActionData& ActionData, const bool bUseCooldown = true);
	/**
	 * Starts a given Cooldown in the parameter by adding it to the stack. This Cooldown is accessible by its ID
	 * @param Cooldown The Cooldown to start (constructing it is possible via "Make Cooldown" Blueprints)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Cooldown")
	bool StartCooldown(UPARAM(ref) FCooldown& Cooldown);
	/**
	 * Ends a given Cooldown in the parameter and removing it from the Cooldown stack.
	 * Can be used in order to clear a Cooldown (The action of this Cooldown will be available for usage again).
	 * @param CooldownID The ID of the Cooldown to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Cooldown")
	void EndCooldown(const FString CooldownID);
	/**
	 * Event called when the cooldown ends
	 * @param Cooldown the Cooldown struct that has ended
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Cooldown", meta = (Keywords = "Ready"))
	void OnEndCooldown(const FCooldown& Cooldown);
	/**
	 * Get a Cooldown from a given ID
	 * @param InID ID of the Cooldown
	 * @param OutCooldown Resulting Cooldown
	 * @return Returns if the Cooldown exists in the Cooldown stack
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Cooldown")
	bool GetCooldown(const FString InID, FCooldown& OutCooldown);
	bool GetCooldown(const FString InID, FCooldown& OutCooldown, int32& Index);
	/**
	 * Gets the Cooldown timers with a given ID (the elapsed and remaining time of the Cooldown)
	 * @param InCooldownID The ID of the Cooldown to look for its Data
	 * @param Elapsed The elapsed time of the Cooldown
	 * @param Remaining The remaining time of the Cooldown
	 * @return Returns if the Cooldown has been found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Cooldown")
	bool GetCooldownTimer(const FString InCooldownID, float& Elapsed, float& Remaining);
	/**
	 * Event called when an Action is executed and its Cooldown is still in the Cooldown stack. 
	 * If this event is called, it means that the actions is still unavailable and wasn't executed.
	 * @param CooldownID The ID of the Cooldown that has been found
	 * @param Elapsed The time elapsed since the Cooldown has been existing in the stack
	 * @param Remaining the remaining time until this Cooldown will end
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Cooldown")
	void OnNotifyCooldown(const FString& CooldownID, const float& Elapsed, const float& Remaining);
	void OnNotifyCooldown_Native(const FString& CooldownID, const float& Elapsed, const float& Remaining);
};
