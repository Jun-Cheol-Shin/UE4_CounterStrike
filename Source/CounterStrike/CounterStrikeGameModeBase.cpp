// Copyright Epic Games, Inc. All Rights Reserved.


#include "CounterStrikeGameModeBase.h"
#include "Engine/World.h"
#include "FPSCharacter.h"
#include "ActorPool.h"
#include "Kismet/GameplayStatics.h"

ACounterStrikeGameModeBase::ACounterStrikeGameModeBase()
{

}

void ACounterStrikeGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void ACounterStrikeGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetCharacter());
	//if (Character)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Get Player!!!!!!!!!!!!!!!!!"));
	//	PlayerArray.Add(Character);
	//}
}

void ACounterStrikeGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ACounterStrikeGameModeBase::ServerSwapCharacter_Implementation()
{

}

void ACounterStrikeGameModeBase::MulticastSwapCharacter_Implementation()
{

}