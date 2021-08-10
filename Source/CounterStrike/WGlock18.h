// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WHandGun.h"
#include "WGlock18.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AWGlock18 : public AWHandGun
{
	GENERATED_BODY()

private:

	FTimerHandle BurstHandle;

	uint8 AttemptBurstShot = 0;
	uint8 ammo;
	bool bIsBurst = false;

protected:

	UPROPERTY(EditAnywhere)
		float BurstDelay = 0.4f;

	UPROPERTY(EditAnywhere)
		USoundBase* BurstSound;
	UPROPERTY(EditAnywhere)
		USoundBase* ActionSound;

	UPROPERTY(EditAnywhere)
		UAnimSequence* SingleShotAnim;

public:
	virtual ~AWGlock18()
	{

	}
	AWGlock18();
	virtual float GetDrawDelay() override;
	virtual void ResetTimeHandle() override;
	virtual void DecreaseAmmo() override;
	virtual void PlayShotSound() override;
	virtual void Fire() override;
	virtual void ShuffleShotAnim() override;
	virtual void Action() override;

	bool GetIsBurst() { return bIsBurst; }
};
