// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WGun.h"
#include "WHandGun.generated.h"

/**
 * 
 */
class UAnimSequence;
UCLASS()
class COUNTERSTRIKE_API AWHandGun : public AWGun
{
	GENERATED_BODY()


protected:
	virtual void SpawnShell() override;

public:

	virtual ~AWHandGun()
	{

	}
	AWHandGun();

	UPROPERTY(EditAnywhere)
		float ShakingValue;

	UPROPERTY(EditAnywhere)
		UAnimSequence* EmptyShotAnim;

	UPROPERTY(EditAnywhere)
		uint8 ShotRecoilStrength;

	virtual void ShakingCamera() override;
	virtual void ShuffleShotAnim() override;
	virtual void RecoilEndVec() override;

	//virtual void SpawnShell() override;
};
