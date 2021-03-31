// Fill out your copyright notice in the Description page of Project Settings.


#include "WUSP.h"
#include "FPSCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"


AWUSP::AWUSP()
{
	SocketName = TEXT("USP_Socket");
	MuzzleSocketName = TEXT("Muzzle_None");
}

void AWUSP::ShuffleShotAnim()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		CurrentPlayingAnim = EmptyShotAnim;
	}

	else
	{
		uint8 random = FMath::RandRange(0, 2);

		if (bIsSilence)
		{
			switch (random)
			{
			case 1:
				CurrentPlayingAnim = AttackSilenceAnim_2;
				break;
			default:
				CurrentPlayingAnim = AttackSilenceAnim_1;
				break;
			}
		}

		else
		{
			switch (random)
			{
			case 1:
				CurrentPlayingAnim = AttackAnim_2;
				break;
			default:
				CurrentPlayingAnim = AttackAnim;
				break;
			}
		}

		if (bIsSilence)
		{
			ShotCount += ShotRecoilStrength * 0.9f;
		}

		else
		{
			ShotCount += ShotRecoilStrength;
		}
	}

	PlayAnim(false);
}

void AWUSP::ResetTimeHandle()
{
	if (GetWorldTimerManager().IsTimerActive(SilenceTimer))
	{
		GetWorldTimerManager().ClearTimer(SilenceTimer);
	}

	Super::ResetTimeHandle();
}

void AWUSP::PlayShotSound()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		UGameplayStatics::SpawnSoundAttached(DryMuzzleSound, Player->GetMesh(), SocketName);
	}

	else
	{
		if (bIsSilence)
		{
			UGameplayStatics::SpawnSoundAttached(SilenceShotSound, Player->GetMesh(), SocketName);
		}

		else
		{
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, Player->GetMesh(), SocketName);
		}
	}
}

float AWUSP::GetDrawDelay()
{
	return DrawAnim->SequenceLength * 0.6f;
}

void AWUSP::Draw(AFPSCharacter* player)
{
	Super::Draw(player);

	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceDraw;
		// Play Anim...	
		PlayAnim(false);
	}
}

void AWUSP::Idle()
{
	Super::Idle();

	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceIdle;
	}
}

void AWUSP::Reload()
{
	Super::Reload();
	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceReload;
	}
	PlayAnim(false);
}

void AWUSP::Action()
{
	if (Player)
	{
		Super::Action();

		if (bIsSilence)
		{
			CurrentPlayingAnim = DetachSilence;
			GetWorld()->GetTimerManager().SetTimer(SilenceTimer, [this]() {
				GetWorld()->GetTimerManager().ClearTimer(SilenceTimer);
				bIsSilence = false;
				MuzzleSocketName = TEXT("Muzzle_None");
			}, Player->GetFPSCharacterStatComponent()->GetCurrentWeapon()->GetActionDelayTime(), false);

		}
		else
		{
			CurrentPlayingAnim = AttachSilence;
			GetWorld()->GetTimerManager().SetTimer(SilenceTimer, [this]() {
				GetWorld()->GetTimerManager().ClearTimer(SilenceTimer);
				bIsSilence = true;
				MuzzleSocketName = TEXT("Muzzle");
			}, Player->GetFPSCharacterStatComponent()->GetCurrentWeapon()->GetActionDelayTime(), false);
		}
		PlayAnim(false);
	}
}