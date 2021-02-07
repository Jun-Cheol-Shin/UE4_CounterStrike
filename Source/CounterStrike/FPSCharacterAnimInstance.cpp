// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacterAnimInstance.h"

#include "WBase.h"
#include "FPSCharacterStatComponent.h"

void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MyChar)
	{
		bCurrentFalling = !MyChar->GetFPSCharacterMovement()->IsGrounded();
		bIsCrouching = MyChar->GetFPSCharacterMovement()->IsCrouchHeld;
		bStartJump = MyChar->GetFPSCharacterMovement()->IsJumpHeld;
		bIsWalk = MyChar->GetFPSCharacterMovement()->IsWalkHeld;

		fForwardVal = MyChar->MoveForwardValue;
		fRightVal = MyChar->MoveRightValue;
		fCurrentLowerRotation = MyChar->CurrentLowerHipsRotation;
		fAimYaw = MyChar->AimOffsetYaw;
		fAimPitch = MyChar->AimOffsetPitch;

		ExitAnimDelayTime = MyChar->DelayTime;
		bIsAttack = MyChar->AttackAnimCall;

		bIsReload = MyChar->bIsReloading;

		if (MyChar->GetFPSCharacterStatComponent()->GetCurrentWeapon())
		{
			eGunNumber = uint8(MyChar->GetFPSCharacterStatComponent()->GetCurrentWeapon()->eGunNumber);
		}

		fStartTime = MyChar->ReloadStartTime;

		deathNum = uint8(MyChar->GetFPSCharacterStatComponent()->GetDeathNum());
	}
}

void UFPSCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	MyChar = Cast<AFPSCharacter>(TryGetPawnOwner()->GetController()->GetCharacter());

	//if (MyChar)
	//{

	//	UE_LOG(LogTemp, Warning, TEXT("Found Character!"));
	//}
	//else
	//{

	//	UE_LOG(LogTemp, Warning, TEXT("Not Found Character!"));
	//}
}

