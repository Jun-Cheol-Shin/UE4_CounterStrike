// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WBase.h"
#include "WThrow.generated.h"

/**
 * 
 */

class UProjectileMovementComponent;
class UDecalComponent;

UCLASS()
class COUNTERSTRIKE_API AWThrow : public AWBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		bool bIsInfinityGrenade = true;

	FTimerHandle ExplodeTimer;

	UPROPERTY(EditAnywhere)
		UParticleSystem* ExplosionParticle;

	UPROPERTY(EditAnywhere)
		float ExplodeTime = 3.f;

	UPROPERTY(EditAnywhere)
		USoundBase* ExplodeSoundQueue;

	bool bIsPossibleThrow = false;
	UProjectileMovementComponent* ProjectileComp;

	UPROPERTY(EditAnywhere)
		UAnimSequence* ThrowAnim;
public:
	AWThrow();

	void ThrowProjectile();

	virtual void ResetTimeHandle() override;
	virtual void Explosion();
	virtual void StopFire() override;
	virtual void Fire() override;
	virtual void Throw();


	void SpawnDecal();
};
