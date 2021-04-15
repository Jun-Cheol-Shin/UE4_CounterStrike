// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CounterStrikeGameModeBase.generated.h"

/**
 * 
 */
class AWBase;
class AFPSCharacter;

//DECLARE_DELEGATE_OneParam(FDele_Single_OneParam, uint8);
UCLASS()
class COUNTERSTRIKE_API ACounterStrikeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:

	TArray<AFPSCharacter*> PlayerArray;
	
public:
	ACounterStrikeGameModeBase();

	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(Server, Reliable)
		void ServerSwapCharacter();
	void ServerSwapCharacter_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSwapCharacter();
	void MulticastSwapCharacter_Implementation();
};
