// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WGun.h"
#include "Global.h"
#include "WSniperRifle.generated.h"

/**
 * 
 */
UCLASS()

//class UUserWidget;
//class UFPSScopeWidget;

class COUNTERSTRIKE_API AWSniperRifle : public AWGun
{
	GENERATED_BODY()


protected:
	FTimerHandle SniperTimerHandle;

	EScope EScopeState;

	UPROPERTY(EditAnywhere)
		bool ScopeWalk = true;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim_3;

	UPROPERTY(EditAnywhere)
		USoundBase* ActionSound;

	UPROPERTY(EditAnywhere)
		float NoScopeHorizontalRecoilValue = 7.f;

	UPROPERTY(EditAnywhere)
		float NoScopeVerticalRecoilValue = 7.f;

	virtual void SpawnShell() override;

public:
	AWSniperRifle();

	virtual void ShakingCamera() override;
	virtual void ShuffleShotAnim() override;
	virtual void Action() override;
	virtual void Fire() override;
	virtual void RecoilEndVec() override;

	void ScopeFunction();

	void CancelScope();

	FORCEINLINE void CancelTimer() { GetWorld()->GetTimerManager().ClearTimer(SniperTimerHandle); }

	EScope GetSniperScopeState();
	void SetSniperScopeState(EScope State);
};
