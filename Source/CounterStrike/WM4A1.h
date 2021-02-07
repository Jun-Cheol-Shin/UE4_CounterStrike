// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRifle.h"
#include "WM4A1.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AWM4A1 : public AWRifle
{
	GENERATED_BODY()

private:
	FTimerHandle SilenceTimer;

	bool bIsSilence = false;

	UPROPERTY(EditAnywhere)
		USoundBase* SilenceShotSound;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceAnim_1;
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceAnim_2;
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceAnim_3;
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttachSilence;
	UPROPERTY(EditAnywhere)
		UAnimSequence* DetachSilence;

	UPROPERTY(EditAnywhere)
		UAnimSequence* SilenceReload;
	UPROPERTY(EditAnywhere)
		UAnimSequence* SilenceIdle;
	UPROPERTY(EditAnywhere)
		UAnimSequence* SilenceDraw;

public:
	AWM4A1();


	virtual void PlayShotSound() override;
 	virtual void ShuffleShotAnim() override;
	virtual void Action() override;
	virtual void Reload() override;
	virtual void Idle() override;
	virtual void Draw(AFPSCharacter* player) override;

	virtual void ResetTimeHandle() override;

	virtual void ChangeRecoilDirection() override;
};
