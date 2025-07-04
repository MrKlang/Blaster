// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 numberOfConnections, FString typeOfMatch, FString lobbyPath)
{
	pathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);
	numPublicConnections = numberOfConnections;
	matchType = typeOfMatch;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();

		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		multiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->multiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		multiplayerSessionsSubsystem->multiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		multiplayerSessionsSubsystem->multiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		multiplayerSessionsSubsystem->multiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		multiplayerSessionsSubsystem->multiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (hostButton)
	{
		hostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (joinButton)
	{
		joinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTeardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* world = GetWorld();

		if (world)
		{
			world->ServerTravel(pathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create a session!")));
		}

		hostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (multiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	for (auto result : SessionResults)
	{
		FString settingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);

		if (settingsValue == matchType)
		{
			multiplayerSessionsSubsystem->JoinSession(result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		joinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();

	if (subsystem)
	{
		IOnlineSessionPtr SessionInterface = subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString adress;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, adress);

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(adress, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		joinButton->SetIsEnabled(true);
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	hostButton->SetIsEnabled(false);

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->CreateSession(numPublicConnections, matchType);
	}
}

void UMenu::JoinButtonClicked()
{
	joinButton->SetIsEnabled(false);

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}
