// KizuEngine Copyright (c) 2019 Jed Fakhfekh. This software is released under the MIT License.


#include "Core/KCharacter.h"
#include "KizuEngine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "FunctionLibrary/KActionFunctionLibrary.h"
#include "FunctionLibrary/KCombatFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AKCharacter::AKCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InitializeStates();
	GetCharacterMovement()->SetIsReplicated(true);

}

void AKCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate Character stats.
	DOREPLIFETIME(AKCharacter, CharacterData);
	DOREPLIFETIME(AKCharacter, LastSpawnedActorRef);
	DOREPLIFETIME(AKCharacter, Inventory);
	DOREPLIFETIME(AKCharacter, AchievedObjectiveRequirements);
	DOREPLIFETIME_CONDITION(AKCharacter, ActiveState, COND_SkipOwner);
}

// Called when the game starts or when spawned
void AKCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AKCharacter::ServerSetCharacterData_Implementation(const FCharacterData& inCharacterData)
{
	CharacterData = inCharacterData;
}

void AKCharacter::ServerSetCurrentHealth_Implementation(const float inValue)
{
	CharacterData.CurrentHealth = inValue;

	OnCurrentHealthChange_Native();
}

void AKCharacter::ServerSetCurrentResource_Implementation(const FString& ResourceName, const float inValue)
{
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName)
			Resource.CurrentValue = inValue;
	}
}


void AKCharacter::ServerSetFaction_Implementation(const uint8 NewFaction)
{
	CharacterData.Faction = NewFaction;
}

bool AKCharacter::GainHealth(const float ValueToGain /*= 10*/)
{
	if (ValueToGain < 0)
		return false;
	float FinalValue = CharacterData.CurrentHealth + ValueToGain;
	if (FinalValue > CharacterData.MaxHealth)
		FinalValue = CharacterData.MaxHealth;
	ServerSetCurrentHealth(FinalValue);
	OnHealthGain_Native(ValueToGain);
	return true;
}

bool AKCharacter::GetResource(const FString ResourceName, FResource& ResultResource)
{
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName) {
			ResultResource = Resource;
			return true;
		}
	}
	return false;
}

bool AKCharacter::GetResourceIndex(const FString ResourceName, int& ResourceIndex)
{
	int Index = 0;
	for (FResource& Resource : CharacterData.Resources) {
		if (Resource.Name == ResourceName) {
			ResourceIndex = Index;
			return true;
		}
		Index++;
	}
	return false;
}

bool AKCharacter::GetResourceCurrentValue(const FString ResourceName, float& ResultValue)
{
	FResource Resource;
	if (GetResource(ResourceName, Resource)) {
		ResultValue = Resource.CurrentValue;
		return true;
	}
	else if (ResourceName == "DEFAULT_HEALTH") {
		ResultValue = CharacterData.CurrentHealth;
		return true;
	}
	return false;
}

bool AKCharacter::GainResource(const FString ResourceName, const float ValueToGain)
{
	FResource Resource;
	if (GetResource(ResourceName, Resource)) {

		float FinalValue = Resource.CurrentValue + ValueToGain;
		if (FinalValue > Resource.MaxValue)
			FinalValue = Resource.MaxValue;
		ServerSetCurrentResource(ResourceName, FinalValue);

		OnResourceGain_Native(ResourceName, ValueToGain);
		return true;
	}
	return false;
}

bool AKCharacter::ConsumeResource(const FString ResourceName, const float ValueToConsume, const bool bCheckEnoughResource)
{
	FResource Resource;
	if (bCheckEnoughResource && !HasEnoughResource(ResourceName, ValueToConsume))
		return false;
	if (GetResource(ResourceName, Resource)) {
		float FinalValue = Resource.CurrentValue - ValueToConsume;
		if (FinalValue < 0 && !Resource.bCanBeBelowZero)
			FinalValue = 0;
		ServerSetCurrentResource(ResourceName, FinalValue);
		OnResourceConsumption_Native(ResourceName, ValueToConsume);
		return true;
	}
	else if (ResourceName == "DEFAULT_HEALTH") {
		ServerApplyDamage(this, ValueToConsume, NULL);
		OnResourceConsumption_Native(ResourceName, ValueToConsume);
		return true;
	}
	return false;
}

void AKCharacter::OnResourceGain_Native(const FString& ResourceName, const float& Value)
{
	OnResourceGain(ResourceName, Value);
}

void AKCharacter::OnResourceConsumption_Native(const FString& ResourceName, const float& Value)
{
	OnResourceConsumption(ResourceName, Value);
}

void AKCharacter::OnHealthGain_Native(const float& Value)
{
	OnHealthGain(Value);
}

void AKCharacter::OnHealthLoss_Native(const float& Value)
{
	OnHealthLoss(Value);
}


void AKCharacter::ApplyDamage_Replicated(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType)
{
	ServerApplyDamage(Target, Damage, DamageType);
}

void AKCharacter::ServerApplyDamage_Implementation(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType)
{
	UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, DamageType);
}

bool AKCharacter::ServerApplyDamage_Validate(AActor* Target, const float Damage, TSubclassOf<UDamageType> DamageType)
{
	return (Damage >= 0);
}

bool AKCharacter::HasEnoughHealth(const float Value /*= 50.f*/)
{
	return (CharacterData.CurrentHealth >= Value);
}

bool AKCharacter::HasEnoughResource(const FString ResourceName, const float Value /*= 50.f*/)
{
	float ResourceCurrentValue;
	if (GetResourceCurrentValue(ResourceName, ResourceCurrentValue))
		return (ResourceCurrentValue >= Value);
	return false;
}

float AKCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Damage = onTakeDamageModifier(Damage, DamageEvent, EventInstigator, DamageCauser);

	float FinalValue = CharacterData.CurrentHealth - Damage;

	if (FinalValue <= 0.f) {
		ServerSetCurrentHealth(0.f);
		ExecuteDeathEvent_Native();
	}
	else ServerSetCurrentHealth(FinalValue);

	OnHealthLoss_Native(Damage);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

float AKCharacter::onTakeDamageModifier_Implementation(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Damage;
}

bool AKCharacter::AddCustomDamage(const FCustomDamage& CustomDamage)
{
	if (!CustomDamageStack.Contains(CustomDamage)) {
		CustomDamageStack.AddUnique(CustomDamage);
		return true;
	}
	return false;
}

bool AKCharacter::GetCustomDamage(const FString InID, FCustomDamage& OutCustomDamage)
{
	OutCustomDamage = FCustomDamage(InID);
	int32 IndexFound = CustomDamageStack.Find(OutCustomDamage);
	if (CustomDamageStack.IsValidIndex(IndexFound)) {
		OutCustomDamage = CustomDamageStack[IndexFound];
		return true;
	}
	return false;
}

bool AKCharacter::EditCustomDamage(const FString InID, const FCustomDamage& InCustomDamage)
{
	FCustomDamage CustomDamage;
	if (GetCustomDamage(InID, CustomDamage)) {
		CustomDamage.Value = InCustomDamage.Value;
		CustomDamage.DamageType = InCustomDamage.DamageType;
		return true;
	}
	return false;
}

void AKCharacter::OnRep_CharacterData()
{
}

void AKCharacter::OnCurrentHealthChange_Native()
{
	OnCurrentHealthChange();
}

void AKCharacter::ExecuteDeathEvent_Native()
{
	if (bPlayDeathMontage && DeathMontage)
		MontagePlay_Replicated(DeathMontage);
	if (bSetStateOnDeath)
		SetCurrentStateFast(DeathState);
	ExecuteDeathEvent();
}

// Called every frame
void AKCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AKCharacter::GetIsNetworked()
{
	return (!UKismetSystemLibrary::IsStandalone(this));
}

void AKCharacter::ServerSpawnActor_Implementation(UClass* Class, const FTransform& Transform)
{
	UWorld* World = GetWorld();
	if (World) {
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;
		LastSpawnedActorRef = World->SpawnActor<AActor>(Class, Transform, ActorSpawnParams);
	}
}

bool AKCharacter::CrosshairTrace(FHitResult& OutHit, FVector& Direction, const ECollisionChannel CollisionChannel /*= ECC_Pawn*/, const float Distance /*= 2000.f*/, const bool bDebug /*= false*/)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		// Get viewport size
		int32 ViewportSizeX;
		int32 ViewportSizeY;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		// Get the position in the middle of the screen (Cross-hair position in the 3D scene)
		FVector CrosshairLocation_Start;
		PlayerController->DeprojectScreenPositionToWorld(ViewportSizeX / 2.f, ViewportSizeY / 2.f, CrosshairLocation_Start, Direction);
		// Calculate the end position where the cross-hair is pointing at by a given distance
		FVector CrosshairLocation_End = ((GetBaseAimRotation().Vector() * Distance) + CrosshairLocation_Start);
		if (bDebug)
			DrawDebugLine(GetWorld(), CrosshairLocation_Start, CrosshairLocation_End, FColor::Red, false, 1, 0, 1);
		if (UWorld* World = GetWorld()) {
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			return World->LineTraceSingleByObjectType(OutHit, CrosshairLocation_Start, CrosshairLocation_End, CollisionChannel, CollisionParams);
		}
	}
	return false;
}

void AKCharacter::AddActorToTargetsArray(AActor* TargetActor)
{
	if (TargetActor) {
		TargetsArray.AddUnique(TargetActor);
		OnAddActorToTargetsArray_Native(TargetActor);
	}
}

void AKCharacter::OnAddActorToTargetsArray_Native(AActor* TargetActor)
{
	UE_LOG(LogKizu, Log, TEXT("Actor [%s] has been added to the target array for the actor [%s]"), *TargetActor->GetName(), *GetName());
	OnAddActorToTargetsArray(TargetActor);
}

void AKCharacter::ClearTargetsArray()
{
	TargetsArray.Empty();
	OnClearTargetsArray_Native();
}

void AKCharacter::OnClearTargetsArray_Native()
{
	UE_LOG(LogKizu, Log, TEXT("Target array has been cleared for the actor [%s]"), *GetName());
	OnClearTargetsArray();
}

void AKCharacter::MontagePlay_Replicated(UAnimMontage* Montage, const float Rate)
{
	if (Montage) {
		ClientMontagePlay(Montage, Rate);
		if (GetIsNetworked())
			ServerMontagePlay(Montage, Rate);
	}
	else UE_LOG(LogKizu, Warning, TEXT("<MontagePlay_Replicated> Cannot play a NULL Montage."));
}

void AKCharacter::ServerMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	MulticastMontagePlay(Montage, Rate);
}

bool AKCharacter::ServerMontagePlay_Validate(UAnimMontage* Montage, const float Rate)
{
	return Montage->IsValidLowLevel();
}

void AKCharacter::ClientMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance->IsValidLowLevel()) {
		AnimInstance->Montage_Play(Montage, Rate);
	}
}

void AKCharacter::MulticastMontagePlay_Implementation(UAnimMontage* Montage, const float Rate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsLocallyControlled() && AnimInstance)
		AnimInstance->Montage_Play(Montage, Rate);
}

void AKCharacter::LocalMontagePlay(UAnimMontage* Montage, APawn* LocalPawn, const float Rate /*= 1.f*/)
{
	if (LocalPawn && Montage)
		if (LocalPawn->IsLocallyControlled() && GetMesh()) {
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance) {
				AnimInstance->Montage_Play(Montage, Rate);
			}
		}
}

void AKCharacter::MontagePlayWithIgnore_Replicated(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate /*= 1.f*/)
{
	if (Montage) {
		ClientMontagePlay(Montage, Rate);
		if (GetIsNetworked() && IsLocallyControlled())
			ServerMontagePlayWithIgnore(Montage, PawnToIgnore, Rate);
	}
	else UE_LOG(LogKizu, Warning, TEXT("<MontagePlayWithIgnore_Replicated> Cannot play a NULL Montage."));
}

void AKCharacter::ServerMontagePlayWithIgnore_Implementation(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate /*= 1.f*/)
{
	MulticastMontagePlayWithIgnore(Montage, PawnToIgnore, Rate);
}

void AKCharacter::MulticastMontagePlayWithIgnore_Implementation(UAnimMontage* Montage, APawn* PawnToIgnore, const float Rate /*= 1.f*/)
{
	if (GetMesh()) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (PawnToIgnore->IsValidLowLevel()) {
			if (!IsLocallyControlled() && AnimInstance && !PawnToIgnore->IsLocallyControlled()) {
				AnimInstance->Montage_Play(Montage, Rate);
			}
		}
		else if (!IsLocallyControlled() && AnimInstance)
			AnimInstance->Montage_Play(Montage, Rate);
	}
}

void AKCharacter::ServerSetTimeDilation_Implementation(const float TimeDilation)
{
	MulticastSetTimeDilation(TimeDilation);
}

void AKCharacter::MulticastSetTimeDilation_Implementation(const float TimeDilation)
{
	CustomTimeDilation = TimeDilation;
}

bool AKCharacter::ExecuteAction(const FActionData& ActionData, const bool bUseCooldown)
{
	if (ActionData.Name == "None") {
		UE_LOG(LogKizu, Warning, TEXT("Invalid ActionData"));
		return false;
	}
	FCooldown Cooldown;
	float TimeElapsed;
	float TimeRemaining;

	// Filter out the montages by their direction and State ( TODO : Notify the user why this can't be executed ). 
	TArray<FMontageData> ValidDirectionMontages;
	if (UKActionFunctionLibrary::FilterMontagesDataByDirection(this, ActionData.MontagesData, ValidDirectionMontages)) {
		TArray<FMontageData> ValidMontages;
		if (UKActionFunctionLibrary::FilterMontagesDataByState(this, ValidDirectionMontages, ValidMontages))
		{
			int32 AnimationsCount = ValidMontages.Num();

			// Check if the character has this Action on cooldown
			if (GetCooldownTimer(ActionData.Name, TimeElapsed, TimeRemaining) && bUseCooldown) {
				OnNotifyCooldown_Native(Cooldown.ID, TimeElapsed, TimeRemaining);
				return false;
			}

			// Check resources and execute this Action
			if (ConsumeResource(ActionData.ResourceName, ActionData.Value, true)) {

				// Check if the ComboCounter is exceeding the combo count in the current action.
				if (ComboCounter >= AnimationsCount || !ValidMontages.IsValidIndex(ComboCounter))
					ComboCounter = 0;

				// Finish by executing the desired animation and increasing the ComboCounter.
				MontagePlay_Replicated(ValidMontages[ComboCounter].AnimMontage, 1.f);
				ComboCounter++;

				if (bUseCooldown) {
					Cooldown = FCooldown(ActionData.Name, ActionData.Cooldown);
					StartCooldown(Cooldown);
				}
				return true;
			}
		}
	}
	return false;
}

bool AKCharacter::AddCooldownToStack(const FCooldown& Cooldown)
{
	if (CooldownStack.Contains(Cooldown))
		return false;
	CooldownStack.AddUnique(Cooldown);
	return true;
}

bool AKCharacter::StartCooldown(FCooldown& Cooldown)
{
	if (UWorld* World = GetWorld()) {
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("EndCooldown"), Cooldown.ID);
		World->GetTimerManager().SetTimer(Cooldown.TimerHandle, TimerDel, 1.f, true, Cooldown.Duration);
		AddCooldownToStack(Cooldown);
		return true;
	}
	return false;
}

void AKCharacter::EndCooldown(const FString CooldownID)
{
	FCooldown Cooldown(CooldownID);
	int32 CooldownIndex;
	if (GetCooldown(CooldownID, Cooldown, CooldownIndex))
	{
		if (UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(Cooldown.TimerHandle);
		CooldownStack.RemoveAt(CooldownIndex, 1, true);
		OnEndCooldown(Cooldown);
	}
}

bool AKCharacter::GetCooldown(const FString InID, FCooldown& OutCooldown)
{
	// Lossy method for BPs
	int32 Index;
	return GetCooldown(InID, OutCooldown, Index);
}

bool AKCharacter::GetCooldown(const FString InID, FCooldown& OutCooldown, int32& Index)
{
	OutCooldown = FCooldown(InID);
	Index = CooldownStack.Find(OutCooldown);
	if (!CooldownStack.IsValidIndex(Index))
		return false;
	OutCooldown = CooldownStack[Index];
	return true;
}

bool AKCharacter::IsCooldownReady(const FString InID)
{
	FCooldown CooldownTemp;
	return !GetCooldown(InID, CooldownTemp);
}

bool AKCharacter::GetCooldownTimer(const FString InCooldownID, float& Elapsed, float& Remaining)
{
	FCooldown Cooldown;
	if (GetCooldown(InCooldownID, Cooldown))
	{
		if (UWorld* World = GetWorld()) {
			Elapsed = World->GetTimerManager().GetTimerElapsed(Cooldown.TimerHandle);
			Remaining = World->GetTimerManager().GetTimerRemaining(Cooldown.TimerHandle);
			return true;
		}
	}
	Elapsed = -1;
	Remaining = -1;
	return false;
}

void AKCharacter::OnNotifyCooldown_Native(const FString& CooldownID, const float& Elapsed, const float& Remaining)
{
	OnNotifyCooldown(CooldownID, Elapsed, Remaining);
}

void AKCharacter::SendReaction(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication)
{
	if (TargetCharacter)
		TargetCharacter->OnReceiveReaction_Native(ReactionData, this, ReactionReplication);
	else UE_LOG(LogKizu, Warning, TEXT("Cannot send a reaction to an invalid character reference."));
}

void AKCharacter::SendReaction_Replicated(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication)
{
	if (ReactionReplication == RR_All || ReactionReplication == RR_SkipSource) {
		ServerSendReaction(ReactionData, TargetCharacter, ReactionReplication);
	}
	else if (ReactionReplication == RR_ServerOnly){
		if (HasAuthority())
			SendReaction(ReactionData, TargetCharacter, ReactionReplication);
	}
	else if (ReactionReplication == RR_SourceOnly) {
		if (IsLocallyControlled())
			SendReaction(ReactionData, TargetCharacter, ReactionReplication);
	}
}

void AKCharacter::ServerSendReaction_Implementation(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication)
{
	SendReaction(ReactionData, TargetCharacter, ReactionReplication);
}

void AKCharacter::MulticastSendReaction_Implementation(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication)
{
	SendReaction(ReactionData, TargetCharacter, ReactionReplication);
}

void AKCharacter::ClientSendReaction_Implementation(const FReactionData& ReactionData, AKCharacter* TargetCharacter, EReactionReplication ReactionReplication)
{
	SendReaction(ReactionData, TargetCharacter, ReactionReplication);
}

void AKCharacter::OnReceiveReaction_Native(const FReactionData& ReactionData, APawn* SourcePawn, EReactionReplication ReactionReplication)
{
	if (UAnimMontage* MontageToPlay = GetReactionMontageToPlay(ReactionData, SourcePawn)) {
		if (ReactionReplication == RR_All)
			MontagePlay_Replicated(MontageToPlay);
		else if (ReactionReplication == RR_ServerOnly)
			ServerMontagePlay(MontageToPlay);
		else if (ReactionReplication == RR_SourceOnly && GetMesh() && SourcePawn->IsLocallyControlled()) {
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance->IsValidLowLevel()) {
				AnimInstance->Montage_Play(MontageToPlay);
			}
		}
		else if (ReactionReplication == RR_SkipSource) {
			if (IsLocallyControlled())
				MontagePlayWithIgnore_Replicated(MontageToPlay, SourcePawn); // Play for everybody besides the source client
		}
	}
	OnReceiveReaction(ReactionData, SourcePawn, ReactionReplication);
}

UAnimMontage* AKCharacter::GetReactionMontageToPlay(const FReactionData& ReactionData, APawn* SourcePawn)
{
	TArray<FReactionMontage_Basic> ResultReactionMontages;
	if (ReactionData.bUseAdvancedReactions) {
		TArray<FReactionMontage_Advanced> FilteredReactionMontages;
		if (SourcePawn) {
			if (UKActionFunctionLibrary::FilterReactionsByDirection(this, SourcePawn, ReactionData.AdvancedReactions, FilteredReactionMontages))
				UKActionFunctionLibrary::FilterReactionsByState(this, static_cast<TArray<FReactionMontage_Basic>>(FilteredReactionMontages), ResultReactionMontages);
		}
		else UKActionFunctionLibrary::FilterReactionsByState(this, static_cast<TArray<FReactionMontage_Basic>>(ReactionData.AdvancedReactions), ResultReactionMontages);
	}
	else {
		UKActionFunctionLibrary::FilterReactionsByState(this, ReactionData.BasicReactions, ResultReactionMontages);
	}
	if (UAnimMontage* MontageToPlay = UKActionFunctionLibrary::GetRandomMontageFromReactionMontages(ResultReactionMontages))
		return MontageToPlay;

	return nullptr;
}

void AKCharacter::SpawnSpawnableAbility_Replicated(TSubclassOf<AKSpawnableAbility> SpawnableAbilityClass, const bool bInitializeMovement, const bool bUseCrosshair, const FName MeshSocketToSpawnAt, const float TargettingRange, const ECollisionChannel CollisionChannel)
{
	FSpawnableAbilitySpawnParams SpawnParam;
	SpawnParam.bInitizalizeMobility = bInitializeMovement;
	SpawnParam.Transform = GetMesh()->DoesSocketExist(MeshSocketToSpawnAt) ? GetMesh()->GetSocketTransform(MeshSocketToSpawnAt) : GetActorTransform();
	if (bUseCrosshair) {
		FHitResult OutHit;
		FVector CrosshairDirection;
		CrosshairTrace(OutHit, CrosshairDirection, CollisionChannel, TargettingRange);
		SpawnParam.InitialDirection = CrosshairDirection;
		SpawnParam.TargetActor = OutHit.GetActor();
	}
	ServerSpawnSpawnableAbility(SpawnableAbilityClass, SpawnParam);
}

void AKCharacter::ServerSpawnSpawnableAbility_Implementation(TSubclassOf<AKSpawnableAbility> SpawnableAbilityClass, const FSpawnableAbilitySpawnParams &SpawnParams)
{
	UKCombatFunctionLibrary::SpawnSpawnableAbility(this, SpawnableAbilityClass, SpawnParams);
}

void AKCharacter::ServerAddItemToInventory_Implementation(const FKItemData& ItemToAdd, const int32 Amount)
{
	UE_LOG(LogKizu, Log, TEXT("Adding %d [%s] to [%s]'s inventory."), Amount, *ItemToAdd.Name, *CharacterData.Name);
	Inventory.AddItem(ItemToAdd);
}

void AKCharacter::ServerRemoveItemFromInventory_Implementation(const FKItemData& ItemToAdd, const int32 Amount)
{
	UE_LOG(LogKizu, Log, TEXT("Removing %d [%s] to [%s]'s inventory."), Amount, *ItemToAdd.Name, *CharacterData.Name);
	Inventory.RemoveItem(ItemToAdd, Amount);
}

bool AKCharacter::ServerRemoveItemFromInventory_Validate(const FKItemData& ItemToAdd, const int32 Amount)
{
	return (Inventory.GetItemCount(ItemToAdd) >= Amount);
}


void AKCharacter::SetCurrentStateFast(const FString& NewState)
{
	if (States.Contains(NewState)) {
		ActiveState = NewState;
		ServerSetCurrentState(NewState);
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot find the state [%s] in the list of the states"), *NewState);
}

void AKCharacter::ServerSetCurrentState_Implementation(const FString& NewState)
{
	if (States.Contains(NewState)) {
		ActiveState = NewState;
	}
	else UE_LOG(LogKizu, Warning, TEXT("Cannot find the state [%s] in the list of the states"), *NewState);
}

void AKCharacter::InitializeStates()
{
	States.AddUnique("Idle");
	States.AddUnique("Hitting");
	States.AddUnique("Casting");
	States.AddUnique("Dodging");
	States.AddUnique("Blocking");
	States.AddUnique("Stunned");
	States.AddUnique("Dead");
}
