// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "KGameInstance.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FOnlineSessionSearchResult_BP
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSearchResult SearchResult;
	FOnlineSessionSearchResult_BP() {
		// Default constructor here
	}
	FOnlineSessionSearchResult_BP(FOnlineSessionSearchResult inSearchResult) {
		SearchResult = inSearchResult;
	}

};

UCLASS()
class KIZUENGINE_API UKGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:


	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handles to registered delegates for creating a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	/** Handles to registered delegate for searching a session */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	/** Handles to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/** Session settings variable to define LAN or not and number of allowed players.. etc */
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;
	/** Session search results */
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/** Game instance Constructor */
	UKGameInstance(const FObjectInitializer& ObjectInitializer);

	/**
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	/**
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession_K(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);


	/**
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Start a server with a given number of players
	*
	*	@param NumebrOfPlayers the number of players to join the session
	*	@param bIsLan if the session can be joined by LAN devices
	*/
	UFUNCTION(BlueprintCallable, Category = "Kizu|Network")
		void StartSession(const int NumebrOfPlayers = 4, const bool bIsLan = true);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Network")
		void FindSessions();

	UFUNCTION(BlueprintCallable, Category = "Kizu|Network")
		void JoinSession_K(FOnlineSessionSearchResult_BP SearchResult);

	UFUNCTION(BlueprintCallable, Category = "Kizu|Network")
		void DestroySession();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category= "Kizu|Network")
		void OnFindSessions(const TArray<FOnlineSessionSearchResult_BP> &SearchResults);
};
