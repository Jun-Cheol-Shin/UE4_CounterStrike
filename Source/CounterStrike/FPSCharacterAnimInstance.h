// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSCharacterAnimInstance.generated.h"

/**
 * 
 */

class AFPSCharacter;

UCLASS()
class COUNTERSTRIKE_API UFPSCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	AFPSCharacter* MyChar;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bCurrentFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bStartJump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsWalk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fForwardVal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fRightVal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fCurrentLowerRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fAimYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fAimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ExitAnimDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsReload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 eGunNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float fStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 deathNum = 0;

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetForwardVal(float Value) { fForwardVal = Value; }

	void GetPlayer(AFPSCharacter* Player) { MyChar = Player; }
};
