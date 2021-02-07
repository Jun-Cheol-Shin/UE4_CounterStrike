// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CounterStrikeGameModeBase.generated.h"

/**
 * 
 */
class AWBase;

DECLARE_DELEGATE_OneParam(FDele_Single_OneParam, uint8);
UCLASS()
class COUNTERSTRIKE_API ACounterStrikeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	FTimerHandle CountdownTimerHandle;
	bool StartGame = false;

	uint8 CountDownTime;

	UPROPERTY(EditAnywhere)
		uint8 PurchaseTime = 10;
	UPROPERTY(EditAnywhere)
		uint8 GameTime = 116;
	
public:
	FDele_Single_OneParam Fuc_DeleSingle_OneParam;

	virtual void StartPlay() override;

	// CountDown Timer...
	void UpdateTimer();
	void AdvanceTimer();
	void CountdownHasFinished();


	uint8 GetPurchaseTime() { return PurchaseTime; }
	uint8 GetGameTime() { return GameTime; }
	uint8 GetCurrentTime() { return CountDownTime; }

	void StartGameMode() { StartGame = true; }
	void ResetGameMode() { StartGame = false; }

	bool GetGameMode() { return StartGame; }
};
