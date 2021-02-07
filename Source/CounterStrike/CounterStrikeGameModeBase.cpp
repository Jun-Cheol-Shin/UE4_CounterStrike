// Copyright Epic Games, Inc. All Rights Reserved.


#include "CounterStrikeGameModeBase.h"
#include "Engine/World.h"

void ACounterStrikeGameModeBase::StartPlay()
{
	Super::StartPlay();

	CountDownTime = PurchaseTime;
	UpdateTimer();
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ACounterStrikeGameModeBase::AdvanceTimer, 1.0f, true);
}

void ACounterStrikeGameModeBase::AdvanceTimer()
{
	--CountDownTime;
	UpdateTimer();
	if (CountDownTime < 1)
	{
		if (StartGame)
		{
			StartGame = false;
			GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
			CountdownHasFinished();
		}

		else
		{
			StartGame = true;
			CountDownTime = GameTime;
		}
	}
}

void ACounterStrikeGameModeBase::CountdownHasFinished()
{
	CountDownTime = PurchaseTime;
	//GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ACounterStrikeGameModeBase::AdvanceTimer, 1.0f, true);
}

void ACounterStrikeGameModeBase::UpdateTimer()
{
	Fuc_DeleSingle_OneParam.ExecuteIfBound(CountDownTime);
}