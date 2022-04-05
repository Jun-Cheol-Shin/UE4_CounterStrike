// Fill out your copyright notice in the Description page of Project Settings.


#include "WM4A1.h"
#include "FPSCharacter.h"
#include "FPSCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"


AWM4A1::AWM4A1()
{
	eWeaponNum = EWeaponNum::E_Rifle;
	eGunNumber = EGunNumber::SMG_MP5;
	bIsSilence = false;

	BackSocketName = "M4_Back";
	HorizontalRandomValue = 0.5f;
}

void AWM4A1::ShuffleShotAnim()
{
	++ShotCount;
	uint8 random = FMath::RandRange(0, 3);

	if (bIsSilence)
	{
		switch (random)
		{
		case 1:
			CurrentPlayingAnim = AttackSilenceAnim_2;
			break;
		case 2:
			CurrentPlayingAnim = AttackSilenceAnim_3;
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
		case 2:
			CurrentPlayingAnim = AttackAnim_3;
			break;
		default:
			CurrentPlayingAnim = AttackAnim;
			break;
		}
	}

	PlayAnim(false);
}

void AWM4A1::ResetTimeHandle()
{
	if (GetWorldTimerManager().IsTimerActive(SilenceTimer))
	{
		GetWorldTimerManager().ClearTimer(SilenceTimer);
	}
	Super::ResetTimeHandle();
}


void AWM4A1::ChangeRecoilDirection()
{
	Super::ChangeRecoilDirection();
}

void AWM4A1::PlayShotSound()
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


void AWM4A1::Action()
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


void AWM4A1::Idle()
{
	Super::Idle();

	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceIdle;
	}
}

void AWM4A1::Draw(AFPSCharacter* player)
{
	Super::Draw(player);

	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceDraw;
		// Play Anim...	
		PlayAnim(false);
	}
}


void AWM4A1::Reload()
{
	Super::Reload();

	if (bIsSilence)
	{
		CurrentPlayingAnim = SilenceReload;
	}

	PlayAnim(false);
}