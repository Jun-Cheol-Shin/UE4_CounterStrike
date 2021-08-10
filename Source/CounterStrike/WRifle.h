// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WGun.h"
#include "WRifle.generated.h"

/**
 * 
 */
UCLASS()

class COUNTERSTRIKE_API AWRifle : public AWGun
{
	GENERATED_BODY()
private:
	//bool WeightSquare = false;

protected:
	UPROPERTY(EditAnywhere)
		float HorizontalRandomValue = 0.7f;

	UPROPERTY(EditAnywhere)
		float RealHitImpactLimit = 6.f;

	UPROPERTY(EditAnywhere)
		float RealHitImpackHorizontalLimit = 5.f;

	UPROPERTY(EditAnywhere)
		float MouseFocusingLimit = 6.f;

	float RecoilWeight = 0.6f;
	float RandomRecoil = 0.f;
	float RandomHorizontalRecoil = 0.f;
	float RecoilHorizontalWeight = 1.f;

	bool Direction = true;
	virtual void SpawnShell() override;

public:
	virtual ~AWRifle()
	{

	}
	AWRifle();

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim_3;

	virtual void ChangeRecoilDirection();
	virtual void Reload() override;

	virtual void ShuffleShotAnim() override;

	float RandomHorizontalDirection();

	virtual void RecoilEndVec() override;
	virtual void StopFire() override;
};
