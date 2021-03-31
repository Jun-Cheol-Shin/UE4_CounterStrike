// Fill out your copyright notice in the Description page of Project Settings.


#include "WGlock18.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacterStatComponent.h"


AWGlock18::AWGlock18()
{
	SocketName = TEXT("Glock_Socket");
}

void AWGlock18::DecreaseAmmo()
{
	if (bIsBurst)
	{
		if (CurrentOneMagazineAmmoCount - 3 >= 0)
		{
			CurrentOneMagazineAmmoCount -= 3;
		}

		else
		{
			CurrentOneMagazineAmmoCount = 0;
		}

	}

	else
	{
		--CurrentOneMagazineAmmoCount;
	}
}

void AWGlock18::ResetTimeHandle()
{
	if (GetWorldTimerManager().IsTimerActive(BurstHandle))
	{
		GetWorldTimerManager().ClearTimer(BurstHandle);
	}

	Super::ResetTimeHandle();
}

void AWGlock18::Fire()
{
	if (bIsBurst)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(BurstHandle))
		{
			return;
		}

		if (CurrentOneMagazineAmmoCount - 3 >= 0)
		{
			ammo = 3;
		}

		else
		{
			ammo = CurrentOneMagazineAmmoCount;
		}

		Super::Fire();

		GetWorld()->GetTimerManager().SetTimer(BurstHandle, [this]() {

			if (AttemptBurstShot >= ammo - 1)
			{
				GetWorld()->GetTimerManager().ClearTimer(BurstHandle);
				AttemptBurstShot = 0;

				GetWorld()->GetTimerManager().SetTimer(BurstHandle, [this]() {

					GetWorld()->GetTimerManager().ClearTimer(BurstHandle);
					StopFire();

				}, BurstDelay, false);
				return;
			}


			GunShotMethod();
			++AttemptBurstShot;

		}, 0.1f, true);
	}

	else
	{
		Super::Fire();
	}

}

void AWGlock18::PlayShotSound()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		UGameplayStatics::SpawnSoundAttached(DryMuzzleSound, Player->GetMesh(), SocketName);
	}

	else
	{
		if (bIsBurst)
		{
			UGameplayStatics::SpawnSoundAttached(BurstSound, Player->GetMesh(), SocketName);
		}
		else
		{
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, Player->GetMesh(), SocketName);
		}
	}
}

void AWGlock18::ShuffleShotAnim()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		CurrentPlayingAnim = EmptyShotAnim;
	}

	else
	{
		uint8 random = FMath::RandRange(0, 2);
		if (bIsBurst)
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

		else
		{
			CurrentPlayingAnim = SingleShotAnim;
		}

		ShotCount += ShotRecoilStrength;
	}

	PlayAnim(false);
}


void AWGlock18::Action()
{
	if (Player)
	{
		Super::Action();
		UGameplayStatics::SpawnSoundAttached(ActionSound, Player->GetMesh(), SocketName);
		bIsBurst = !bIsBurst;
	}
}

float AWGlock18::GetDrawDelay()
{
	return DrawAnim->SequenceLength * 0.6f;
}

