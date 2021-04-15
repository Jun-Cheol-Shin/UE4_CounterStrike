// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "FPSDedicatedGameSession.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AFPSDedicatedGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	virtual void RegisterServer() override;
	
};
