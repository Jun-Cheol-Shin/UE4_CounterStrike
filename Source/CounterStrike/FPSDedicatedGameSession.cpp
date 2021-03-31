// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSDedicatedGameSession.h"
#include "Online.h"

void AFPSDedicatedGameSession::RegisterServer()
{
	UWorld* World = GetWorld();

	IOnlineSessionPtr SessionInt = Online::GetSessionInterface();

	FOnlineSessionSettings Settings;
	Settings.NumPublicConnections = 3;
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true;
	Settings.bIsLANMatch = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	SessionInt->CreateSession(0, GameSessionName, Settings);
}