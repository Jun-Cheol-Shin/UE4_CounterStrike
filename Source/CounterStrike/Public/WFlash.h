// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WThrow.h"
#include "WFlash.generated.h"

/**
 * 
 */
class UDecalComponent;
class AActorPool;

UCLASS()
class COUNTERSTRIKE_API AWFlash : public AWThrow
{
	GENERATED_BODY()

protected:

	AActorPool* ExplosionPool;

	UPROPERTY(EditAnywhere)
		USoundBase* BangSound;

public:
	AWFlash();

	virtual void Explosion() override;

	void FlashCheck();
};
