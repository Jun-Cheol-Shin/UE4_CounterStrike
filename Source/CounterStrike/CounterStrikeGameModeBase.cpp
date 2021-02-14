// Copyright Epic Games, Inc. All Rights Reserved.


#include "CounterStrikeGameModeBase.h"
#include "Engine/World.h"

ACounterStrikeGameModeBase::ACounterStrikeGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/BluePrint/Character/BP_FPSCharacter.BP_FPSCharacter_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

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