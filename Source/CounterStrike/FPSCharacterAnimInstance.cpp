// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacterAnimInstance.h"

#include "WBase.h"
#include "GameFramework/Actor.h"
#include "FPSCharacterStatComponent.h"

void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MyChar)
	{
		bStartJump = MyChar->IsJumpHeld;
		bCurrentFalling = MyChar->GetMovementComponent()->IsFalling();

		bIsCrouching = MyChar->IsCrouchHeld;
		bIsWalk = MyChar->IsWalkHeld;

		fForwardVal = MyChar->MoveForwardValue;
		fRightVal = MyChar->MoveRightValue;

		fCurrentLowerRotation = MyChar->CurrentLowerHipsRotation;
		fAimYaw = MyChar->AimOffsetYaw;
		fAimPitch = MyChar->AimOffsetPitch;

		ExitAnimDelayTime = MyChar->DelayTime;
		bIsAttack = MyChar->AttackAnimCall;

		bIsReload = MyChar->bIsReloading;

		eGunNumber = MyChar->CurrentAnimationWeaponNumber;
		fStartTime = MyChar->ReloadStartTime;
		
		if (MyChar->GetFPSCharacterStatComponent())
		{
			deathNum = uint8(MyChar->GetFPSCharacterStatComponent()->GetDeathNum());
		}
	}
}


void UFPSCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	//MyChar = Cast<AFPSCharacter>(TryGetPawnOwner()->GetController()->GetCharacter());

	//if (MyChar)
	//{

	//	UE_LOG(LogTemp, Warning, TEXT("Found Character!"));
	//}
	//else
	//{

	//	UE_LOG(LogTemp, Warning, TEXT("Not Found Character!"));
	//}
}