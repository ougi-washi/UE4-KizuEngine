// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Core/Combat/KizuCombat.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "KAction.generated.h"

UENUM(BlueprintType)
enum EActionDirection
{
	AD_Any UMETA(DisplayName = "Any"),
	AD_Forward UMETA(DisplayName = "Forward"),
	AD_Backward UMETA(DisplayName = "Backward"),
	AD_Right UMETA(DisplayName = "Right"),
	AD_Left UMETA(DisplayName = "Left"),
};

UENUM(BlueprintType)
enum ERelativePosition
{
	// Creating another coordinate system so we can use it to add FrontRight / BackLeft for example.
	RP_Front UMETA(DisplayName = "Front"),
	RP_Back UMETA(DisplayName = "Back"),
	RP_Right UMETA(DisplayName = "Right"),
	RP_Left UMETA(DisplayName = "Left"),
};

UENUM(BlueprintType)
enum EReactionReplication
{
	RR_All UMETA(DisplayName = "All"),
	RR_SourceOnly UMETA(DisplayName = "Source Only"),
	RR_ServerOnly UMETA(DisplayName = "Server Only"),
	RR_SkipSource UMETA(DisplayName = "Skip Source")
};

UENUM(BlueprintType)
enum EDirectionMode
{
	CameraRotation,
	CharacterRotation
};

USTRUCT(Blueprintable, BlueprintType)
struct FMontageData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	UAnimMontage* AnimMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	TEnumAsByte<EActionDirection> Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	TEnumAsByte<EDirectionMode> DirectionMode;

	/** Valid states that would make this action to be eligible to execute */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	TArray<FString> ValidStates = {"Idle"};
};

USTRUCT(Blueprintable, BlueprintType)
struct FActionData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** Name of the Action. Make sure the Name of the Action is unique as it's the Key for the Cooldown Stack.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString Name = "None";

	/** Animation Montage to play for this Action (Define one montage if it's a single animation or multiple if it's a Combination of montages (Combo System)) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	TArray<FMontageData> MontagesData;

	/** Resource name to use. In order to consume Health as resource, please use "DEFAULT_HEALTH" */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString ResourceName = "None";

	/** Value of the resources to consume */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Value = 10;

	/** Cooldown of the Action to be able to use it again This doesn't work on the Combo System. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	float Cooldown = 5.f;

	/** Name of the action to display */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Display)
	FName DisplayName = "Action_Name";

	/** The action icon to display*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Display)
	UTexture2D *ActionIcon = nullptr;

	/** The action description to display */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Display)
	FText Description;
};

USTRUCT(Blueprintable, BlueprintType)
struct FReactionMontage_Basic
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
		UAnimMontage* AnimMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
		TArray<FString> ValidStates = { "Idle" };
};

USTRUCT(Blueprintable, BlueprintType)
struct FReactionMontage_Advanced : public FReactionMontage_Basic
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	TEnumAsByte<ERelativePosition> DamageSourceRelativePosition;

	/** In order to filter out which AnimMontage to pick exactly, we can use a Filter (e.g. by intensity "StrongHit", "NormalHit")*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString Filter = "None";
};

USTRUCT(Blueprintable, BlueprintType)
struct FReactionData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	FString Name = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base, meta = (EditCondition = "!bUseAdvancedReactions"))
	TArray<FReactionMontage_Basic> BasicReactions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base)
	uint8 bUseAdvancedReactions : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Base, meta=(EditCondition = "bUseAdvancedReactions"))
	TArray<FReactionMontage_Advanced> AdvancedReactions;
};

USTRUCT(BlueprintType)
struct FCooldown
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ID = "None";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Duration = 0.f;

	FTimerHandle TimerHandle;

	FORCEINLINE bool operator==(const FCooldown& rhs) const
	{
		return (rhs.ID == this->ID);
	}
	FCooldown() {
		// Default Constructor
	}
	FCooldown(const FString& InID) {
		this->ID = InID;
	}
	FCooldown(const FString& InID, const float& InDuration) {
		this->ID = InID;
		this->Duration = InDuration;
	}

	static bool FindByID(const TArray<FCooldown>& InCooldowns, const FString& InID, FCooldown& OutCooldown) {
		for (int i = 0; i < InCooldowns.Num(); i++) {
			OutCooldown = InCooldowns[i];
			if (OutCooldown.ID == InID) {
				return true;
			}
		}
		return false;
	}
};

USTRUCT(BlueprintType)
struct FReactionSendingData
{
	GENERATED_BODY()

public:

	/** Whether send a reaction to the targeted character or not. (It won't receive a reaction unless it's a character) */
	UPROPERTY(EditAnywhere, Category = Reaction, meta = (Tooltip = "Whether send a reaction to the targeted character or not. (It won't receive a reaction unless it's a character)"))
	uint8 bSendReaction : 1;

	/** Whether to use a data table specific to the player character or a generic one set here for the reaction system */
	UPROPERTY(EditAnywhere, Category = Reaction, meta = (EditCondition = "bSendReaction", Tooltip = "Whether to use a data table specific to the player character or a generic one set here for the reaction system"))
	uint8 bUseCharacterReactionDataTable : 1;

	/** Data table to use as a source of the reaction */
	UPROPERTY(EditAnywhere, Category = Reaction, meta = (EditCondition = "bSendReaction && !bUseCharacterReactionDataTable", Tooltip = "Data table to use as a source of the reaction"))
	UDataTable* ReactionDataTable;

	/** Data table Row Name for the reaction */
	UPROPERTY(EditAnywhere, Category = Reaction, meta = (EditCondition = "bSendReaction", Tooltip = "Data table Row Name for the reaction"))
	FString ReactionRowName = "None";

	/** This is an experimental version of Sending a reaction which causes to have an instant reaction locally and a late reaction on the server depending on the bandwidth latency */
	UPROPERTY(EditAnywhere, Category = Reaction, AdvancedDisplay, meta = (Tooltip = "This is an experimental version of Sending a reaction which causes to have an instant reaction locally and a late reaction on the server depending on the bandwidth latency"))
	uint8 bSmoothReaction : 1;

	FReactionSendingData() {
		bUseCharacterReactionDataTable = true;
	}
};


UCLASS(Abstract)
class KIZUENGINE_API AKAction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
