// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WThrow.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "WGrenade.generated.h"

/**
 * 
 */

class URadialForceComponent;
class UDecalComponent;
class AActorPool;

UCLASS()
class COUNTERSTRIKE_API AWGrenade : public AWThrow
{
	GENERATED_BODY()

protected:

	AActorPool* ExplosionPool;

	UPROPERTY(EditAnywhere)
		URadialForceComponent* RadialForce;

public:
	AWGrenade();

	float GetDistance(AFPSCharacter* DamagedActor);
	virtual void Explosion() override;
	
};
