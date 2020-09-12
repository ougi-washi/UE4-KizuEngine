// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/KGameInstance.h"

UKGameInstance::UKGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UKGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UKGameInstance::OnStartOnlineGameComplete);
}

bool UKGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	return false;
}

void UKGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UKGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
}
