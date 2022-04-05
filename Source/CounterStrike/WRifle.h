// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WGun.h"
#include "WRifle.generated.h"

UENUM()
enum class EC_Direction
{
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2
};

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

	// 수직 한계
	UPROPERTY(EditAnywhere)
		float RealHitImpactLimit = 6.f;

	// 수평 한계
	UPROPERTY(EditAnywhere)
		float RealHitImpackHorizontalLimit = 4.f;

	// 랜덤 리코일 한계
	UPROPERTY(EditAnywhere)
		float MouseFocusingLimit = 6.f;

	float RecoilWeight = 0.6f;
	float RandomRecoil = 0.f;
	float RandomHorizontalRecoil = 0.f;
	float RecoilHorizontalWeight = 1.f;


	EC_Direction dir = EC_Direction::RIGHT;
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

	void RandomHorizontalDirection();

	virtual void RecoilEndVec() override;
	virtual void StopFire() override;
};
