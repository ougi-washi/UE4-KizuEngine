// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Core/KAction.h"
#include "Core/Combat/KizuCombat.h"
#include "Core/Inventory/KInventory.h"
#include "KCharacter.generated.h"

class AKBuff;
class AKSpawnableAbility;
class UDataTable;

USTRUCT(BlueprintType)
struct FKResource {

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

	/** Whether the value can be below 0 or not. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	bool bCanBeBelowZero = false;
};


USTRUCT(BlueprintType)
struct FKResourceRegeneration {

	GENERATED_USTRUCT_BODY()

public:

	/** The custom resource name (DEFAULT_HEALTH for health) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "None";
	/** The value to regenerate every second */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float RegenValue = 1.f;
	/** How much does it take to trigger the regeneration (default = every 1 second) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float TickingRate = 1.f;
	/** Timer handle to manage this resource regeneration ticking */
	FTimerHandle TimerHandle;
};

USTRUCT(BlueprintType)
struct FKCharacterData 
{
	GENERATED_USTRUCT_BODY()

public:
	/** The character name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	FString Name = "Character_Name";

	/** The character max health. The current health can never exceed this amount */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float MaxHealth = 100.f;

	/** The character current health. When reduced to 0, they are considered dead. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	float CurrentHealth = 100.f;

	/** Custom resources array (Examples: Energy, Mana, Armors..) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResource> Resources;

	/** Resource Regeneration array (Name has to be available in Resources) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data")
	TArray<FKResourceRegeneration> ResourcesRegen;

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

	// Custom Damage to fill, clear and use during Gameplay.
	UPROPERTY()
	TArray<FCustomDamage> CustomDamageStack;

public:	

	/** The character's Data, containing all kind of general stats that vary during the Gameplay.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_CharacterData, Category = "Kizu|Character|Data")
	FKCharacterData CharacterData;

	/** If the character plays the death montage on death */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data|Death")
	bool bPlayDeathMontage = false;

	/** This death montage is played on the death of the character (Current health equals 0)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bPlayDeathMontage"), Category = "Kizu|Character|Data|Death")
	UAnimMontage* DeathMontage;

	/** If the a "Dead" state to be set on death */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data|Death")
	bool bSetStateOnDeath = false;

	/** If the a "Dead" state to be set on death */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Kizu|Character|Data|Death", meta= (EditCondition = "bSetStateOnDeath"))
	FString DeathState = "Dead";

	/** The Cooldown stack that holds are the Cooldowns */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Kizu|Character|Temp")
	TArray<FCooldown> CooldownStack;

	/** Items stack (Inventory array) */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = "Kizu|Character|Temp")
	FKInventoryData Inventory;

	/** This represents the character possible states (TODO : Make it as a struct and give each state a priority for later execution of events and effect)*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kizu|Character|Data|State")
	TArray<FString> States;

	/** This represents the character state */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Kizu|Character|Data|State")
	FString ActiveState = "Idle";

	/** The achieved requirements */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = "Kizu|Character|Data|Objectives")
	TArray<FString> AchievedObjectiveRequirements;

	/** The Data table that represents the list of reactions specific to this character */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kizu|Character|Data|Reactions")
	UDataTable* ReactionDataTable;

	/**
	 * TEMP VALUES START
	 */

	/** This is a temp variable used for the Combo Systems, this is shared for now among all the combos (a queue has to be created later on). */
	int32 ComboCounter = 0;
	/* The array of the targets to interact with, damage or such. TODO : change to TSharedPointer */
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> TargetsArray;
	// Temp pointer to the last spawned Actor. TODO : change to TSharedPointer
	UPROPERTY(Replicated)
	AActor* LastSpawnedActorRef;

	/**
	 * TEMP VALUES END
	 */


	// Sets default values for this character's properties
	AKCharacter(const FObjectInitializer& ObjectInitializer);

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|General|Network")
	bool GetIsNetworked();

	/*
	* Replicated Actor Spawn
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|General")
	void ServerSpawnActor(UClass* Class, const FTransform& Transform);

	/** Traces from the cross-hair (middle of the screen to a certain range passed into the arguments). Does not replicate.
	 * @param OutHit - Is the output HitResult when the trace hits.
	 * @param Direction - Is the direction of the screen.
	 * @param CollisionChannel - Is the collision channel to be able to hit in the trace.
	 * @param Distance - Is the Distance of the trace to accept (Range).
	 * @param bDebug - Whether display the trace or not (for debug purpose).
	 * @return Whether it has hit or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|General|Trace")
	bool CrosshairTrace(FHitResult& OutHit, FVector& Direction, const ECollisionChannel CollisionChannel = ECC_Pawn, const float Distance = 2000.f, const bool bDebug = false);

	/** Add an actor to the saved targets array for later  usage. */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|General|Targetting")
	void AddActorToTargetsArray(AActor* TargetActor);

	/** event called when an actor has been added to the targets array. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|General|Targetting")
	void OnAddActorToTargetsArray(AActor* TargetActor);
	virtual void OnAddActorToTargetsArray_Native(AActor* TargetActor);

	/** Clear (Emptying) the targets array. This also calls "OnClearTargetArray()".*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|General|Targetting")
	void ClearTargetsArray();

	/** Event called when the targets array is cleared. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|General|Targetting")
	void OnClearTargetsArray();
	virtual void OnClearTargetsArray_Native();

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
	void ServerSetCharacterData(const FKCharacterData& inCharacterData);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kizu|Character|Data")
	float onTakeDamageModifier(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	/**
	* Get a resource from the character data
	* @param ResourceName The resource name to look for in the Array
	* @param ResultResource The resulting resource
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Data")
	bool GetResource(const FString ResourceName, FKResource& ResultResource);

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
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Combat")
	void ApplyDamage_Replicated(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType);

	/** Apply damage to an Actor server side (replicated).*/
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

	/** Apply regeneration effect */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Combat")
	void ApplyRegen(const FKResourceRegeneration& ResourceRegen);

	/** Initialize All regeneration effects and create their timer handles */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Combat")
	void InitAllRegens(const TArray<FKResourceRegeneration>& ResourcesRegen);




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
	 * Play Montage on server and all clients aside from the owner of the character and the owner (Through the AnimInstance of the main Mesh)
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
	 * Play Montage on all clients aside from the owner of the character (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Character|Animation")
	void MulticastMontagePlay(UAnimMontage* Montage, const float Rate = 1.f);

	/**
	 * Play Montage on the client's pawn given in the argument call (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param LocalPawn The actual pawn to play for
 	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Animation")
	void LocalMontagePlay(UAnimMontage* Montage, APawn* LocalPawn, const float Rate = 1.f);

	/**
	 * Play Montage on server and all clients except the ones in the ignore array (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param PawnsToIgnore The ignore array of the client's pawns 
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Animation")
	void MontagePlayWithIgnore_Replicated(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate = 1.f);

	/**
	 * Play Montage on server and all clients except the ones in the ignore array and the owner (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param PawnsToIgnore The ignore array of the client's pawns
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Animation")
	void ServerMontagePlayWithIgnore(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate = 1.f);

	/**
	 * Play Montage on all clients except the ones in the ignore array and the owner (Through the AnimInstance of the main Mesh)
	 * @param Montage The montage to be played by the AnimInstance
	 * @param PawnsToIgnore The ignore array of the client's pawns
	 * @param Rate Play rate of the Montage
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Character|Animation")
	void MulticastMontagePlayWithIgnore(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate = 1.f);

	/**
	 * Set time dilation on the server (Replicated)
	 * @param TimeDilation new input time dilation to apply
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Kizu|Character|Animation")
	void ServerSetTimeDilation(const float TimeDilation);

	/**
	 * Set time dilation on the clients
	 * @param TimeDilation new input time dilation to apply
	 */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Kizu|Character|Animation")
	void MulticastSetTimeDilation(const float TimeDilation);

	/**
	 * Actions, Reactions and Cooldowns
	 */

	/**
	 * Executes a given action while choosing if it should be added to the Cooldown stack or not (Replicated)
	 * @param ActionData The action to execute in this function
	 * @param bUseCooldown If a Cooldown will be added to the stacks
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Action")
	bool ExecuteAction(const FActionData& ActionData, const bool bUseCooldown = true);

	/**
	 * Starts a given Cooldown in the parameter by adding it to the stack. This Cooldown is accessible by its ID
	 * @param Cooldown The Cooldown to start (constructing it is possible via "Make Cooldown" Blueprints)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Cooldown")
	bool StartCooldown(UPARAM(ref) FCooldown& Cooldown);

	/**
	 * Ends a given Cooldown in the parameter and removing it from the Cooldown stack.
	 * Can be used in order to clear a Cooldown (The action of this Cooldown will be available for usage again).
	 * @param CooldownID The ID of the Cooldown to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Cooldown")
	void EndCooldown(const FString CooldownID);

	/**
	 * Event called when the cooldown ends
	 * @param Cooldown the Cooldown struct that has ended
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Cooldown", meta = (Keywords = "Ready"))
	void OnEndCooldown(const FCooldown& Cooldown);

	/**
	 * Get a Cooldown from a given ID
	 * @param InID ID of the Cooldown
	 * @param OutCooldown Resulting Cooldown
	 * @return Returns if the Cooldown exists in the Cooldown stack
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Cooldown")
	bool GetCooldown(const FString InID, FCooldown& OutCooldown);
	bool GetCooldown(const FString InID, FCooldown& OutCooldown, int32& Index);

	/**
	 * Returns if the cooldown is ready.
	 * The cooldown is ready means that the cooldown isn't available in the Cooldown stack.
	 * @Param InID The ID of the Cooldown to look for
	 * @return Returns if the cooldown is ready or not
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Cooldown")
	bool IsCooldownReady(const FString InID);

	/**
	 * Gets the Cooldown timers with a given ID (the elapsed and remaining time of the Cooldown)
	 * @param InCooldownID The ID of the Cooldown to look for its Data
	 * @param Elapsed The elapsed time of the Cooldown
	 * @param Remaining The remaining time of the Cooldown
	 * @return Returns if the Cooldown has been found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Kizu|Character|Cooldown")
	bool GetCooldownTimer(const FString InCooldownID, float& Elapsed, float& Remaining);

	/**
	 * Event called when an Action is executed and its Cooldown is still in the Cooldown stack. 
	 * If this event is called, it means that the actions is still unavailable and wasn't executed.
	 * @param CooldownID The ID of the Cooldown that has been found
	 * @param Elapsed The time elapsed since the Cooldown has been existing in the stack;:
	 * @param Remaining the remaining time until this Cooldown will end
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Cooldown")
	void OnNotifyCooldown(const FString& CooldownID, const float& Elapsed, const float& Remaining);
	void OnNotifyCooldown_Native(const FString& CooldownID, const float& Elapsed, const float& Remaining);

	/**
	 * Send a reaction to a specific character and making the target character given into argument execute specific animation. (Does not replicate)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Reaction")
	void SendReaction(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication = EReactionReplication::RR_All);
	/**
	 * Send a reaction to a specific character and making the target character given into argument execute specific animation. (Replicates)
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Reaction")
	void SendReaction_Replicated(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication = EReactionReplication::RR_All);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Reaction")
	void ServerSendReaction(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication = EReactionReplication::RR_All);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Kizu|Character|Reaction")
	void MulticastSendReaction(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication = EReactionReplication::RR_All);

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Kizu|Character|Reaction")
	void ClientSendReaction(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication = EReactionReplication::RR_All);

	UFUNCTION(BlueprintImplementableEvent, Category = "Kizu|Character|Reaction")
	void OnReceiveReaction(const FReactionData& ReactionData, APawn* SourcePawn, EReactionReplication ReactionReplication);
	virtual void OnReceiveReaction_Native(const FReactionData& ReactionData, APawn* SourcePawn, EReactionReplication ReactionReplication = EReactionReplication::RR_All);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Reaction")
	UAnimMontage* GetReactionMontageToPlay(const FReactionData& ReactionData, APawn* SourcePawn);

	/**
	 * Specific Spawnables (To add here further spawn-ables classes if required to be initialized in a specific way.
	 * I am doing this due to being obliged in some cases. It's really not the approach I am willing to take and I am really thinking about changing it.
	 * I believe that this class should be independent from all other classes related to combat, however, I am limited at some point and I am willing to have things work properly 
	 * even if a unnoticeable small lack of performance exists.
	 * I will try to remove all of what's inside this section and make a proper independent manager for the data that has to be transmitted on spawn 
	 * or make the spawn-able classes initialize themselves with data taken from the owner AActor.
	 */
	
	/** 
	 * Spawn a Spawnable ability on the server only if the character is controlled locally. That said, it's safe to call it in an AnimNotify.
	 * @param SpawnableAbilityClass - The Spawnable Ability class to be spawned from
	 * @param bInitialize - If the Spawnable Ability's movement will be initialized
	 * @param bUseCrossHair - If the initialization will be managed by the Cross-hair or not.
	 * @param MeshSocketToSpawnAt - the name of the socket from where the ability is going to be spawned/
	 */
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|Spawnable")
	void SpawnSpawnableAbility_Replicated(TSubclassOf<AKSpawnableAbility> SpawnableAbilityClass, const bool bInitializeMovement = true, const bool bUseCrosshair = true, const FName MeshSocketToSpawnAt = "None", const float Range = 2000.f, const ECollisionChannel CollisionChannel = ECC_Pawn);

	/**
	 *
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Spawnable")
	void ServerSpawnSpawnableAbility(TSubclassOf<AKSpawnableAbility> SpawnableAbilityClassconst, const FSpawnableAbilitySpawnParams& SpawnParams);

	/**
	 * Inventory
	 */

	/**
	 * Call adding an item given into the argument to the inventory (Replicates on the server)
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|Inventory")
	void ServerAddItemToInventory(const FKItemData &ItemToAdd, const int32 Amount);

	/**
	 * Call adding an item given into the argument to the inventory (Replicates on the server)
	 */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Kizu|Character|Inventory")
	void ServerRemoveItemFromInventory(const FKItemData& ItemToAdd, const int32 Amount);

	/**
	 * Character States
	 */

	 /** Set the current state of the character to the one given into the argument. (On self first, then server/other clients)*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Character|State")
	void SetCurrentStateFast(const FString& NewState);

	/** Set the current state of the character to the one given into the argument. (On Server) */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Kizu|Character|State")
	void ServerSetCurrentState(const FString &NewState);

	/**
	 * Misc & Initializations
	 */

	/** Initialize the default states */
	void InitializeStates();
};
