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
	bool WeightSquare = false;

	UPROPERTY(EditAnywhere)
		float HorizontalRandomValue = 0.5f;

	UPROPERTY(EditAnywhere)
		float RealHitImpactLimit = 6.f;

	UPROPERTY(EditAnywhere)
		float RealHitImpackHorizontalLimit = 5.f;

	UPROPERTY(EditAnywhere)
		float MouseFocusingLimit = 6.f;

	float RecoilWeight = 0.7f;
	float RandomRecoil = 0.f;
	float RandomHorizontalRecoil = 0.f;
	float RecoilHorizontalWeight = 1.f;

protected:
	bool Direction = true;
	virtual void SpawnShell() override;

public:
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim_3;

	AWRifle();


	virtual void ChangeRecoilDirection();
	virtual void Reload() override;

	virtual void ShuffleShotAnim() override;

	float RandomHorizontalDirection();

	virtual void RecoilEndVec() override;
	virtual void StopFire() override;
};
