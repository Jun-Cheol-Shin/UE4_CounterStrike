// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WHandGun.h"
#include "WUSP.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AWUSP : public AWHandGun
{
	GENERATED_BODY()

private:
	bool bIsSilence = false;

protected:
	FTimerHandle SilenceTimer;

	UPROPERTY(EditAnywhere)
		USoundBase* SilenceShotSound;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceAnim_1;
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceAnim_2;
	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackSilenceEmpty;
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
	virtual ~AWUSP()
	{

	}
	AWUSP();
	virtual float GetDrawDelay() override;

	virtual void ShuffleShotAnim() override;
	virtual void ResetTimeHandle() override;
	virtual void PlayShotSound() override;
	virtual void Action() override;
	virtual void Idle() override;
	virtual void Reload() override;

	virtual void Draw(AFPSCharacter* player) override;
};
