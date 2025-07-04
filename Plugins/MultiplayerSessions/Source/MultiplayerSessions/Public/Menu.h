// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 numberOfConnections = 4, FString typeOfMatch = FString(TEXT("FreeForAll")), FString lobbyPath = "/Game/ThirdPerson/Maps/Lobby");

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* hostButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* joinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTeardown();

	class UMultiplayerSessionsSubsystem* multiplayerSessionsSubsystem;

	int32 numPublicConnections{4};
	FString matchType {TEXT("FreeForAll")};
	FString pathToLobby{ TEXT("") };
};
