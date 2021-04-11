// Fill out your copyright notice in the Description page of Project Settings.


#include "WRifle.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"

AWRifle::AWRifle()
{
	eWeaponNum = EWeaponNum::E_Rifle;

	bCanAutoFire = true;
	//eGunNumber = EGunNumber::AR_AK;

	RunSpeedRatio = 0.9f;

	PenatrateDecreaseDistanceRatio = 2.5f;
}


void AWRifle::ShuffleShotAnim()
{
	uint8 random = FMath::RandRange(0, 3);

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
	++ShotCount;
	Super::ShuffleShotAnim();
}


void AWRifle::StopFire()
{
	Super::StopFire();


	RecoilWeight = 0.7f;
	RandomRecoil = 0.f;

	RandomHorizontalRecoil = 0.f;
	RecoilHorizontalWeight = 1.f;
	Direction = true;
	//WeightSquare = false;
}

void AWRifle::Reload()
{
	if (AmmoCount == 0)
		return;

	Super::Reload();

	RecoilWeight = 0.7f;
	RandomRecoil = 0.f;

	RandomHorizontalRecoil = 0.f;
	RecoilHorizontalWeight = 1.f;
	Direction = false;
	//WeightSquare = false;
}

void AWRifle::ChangeRecoilDirection()
{
	// go left = true
	// go right = false

	if (ShotCount > 20)
	{
		Direction = false;
	}

	else if (ShotCount > 14)
	{
		Direction = true;
	}

	else if (ShotCount > 8)
	{
		Direction = false;
	}

	else if (ShotCount > 5)
	{
		Direction = true;
	}
}



float AWRifle::RandomHorizontalDirection()
{
	float retval = 0.f;

	ChangeRecoilDirection();

	if (RandomRecoil < MouseFocusingLimit)
	{
		retval = FMath::RandRange(-1.f, 1.f);
	}

	else
	{
		if (!Direction && RandomHorizontalRecoil < RealHitImpackHorizontalLimit)
		{
			RandomHorizontalRecoil += FMath::RandRange(HorizontalRandomValue * 1.5f, HorizontalRandomValue * 1.7f);
		}

		else if(Direction && RandomHorizontalRecoil > -RealHitImpackHorizontalLimit)
		{
			RandomHorizontalRecoil -= FMath::RandRange(HorizontalRandomValue * 2.15f, HorizontalRandomValue * 2.5f);
		}

		retval = RandomHorizontalRecoil;
	}

	return retval;
}

void AWRifle::RecoilEndVec()
{

	if(!RandomRecoilFlag)
	{
		//FirstShotRot = FirstShotRotBackUp;

		switch (ShotCount)
		{
		case 0:
			Player->AddControllerPitchInput(-RandomRecoil * 0.1f);
			break;
		case 1:
		case 2:
		case 3:
			RandomRecoil = FMath::RandRange(0.8f, 1.15f);
			Rotation.Pitch += RandomRecoil;
			RandomRecoil += RecoilWeight;
			Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
			break;

		default:
			if (RandomRecoil < RealHitImpactLimit)
			{
				RandomRecoil += RecoilWeight;
				RecoilWeight += .1f;

				if (RandomRecoil > RealHitImpactLimit)
				{
					RandomRecoil = RealHitImpactLimit;
				}

				Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
			}


			if (Player->IsCrouchHeld)
			{
				Rotation.Pitch += RandomRecoil * 0.6f;
			}

			else
			{
				Rotation.Pitch += RandomRecoil;
			}

			break;
		}

		if (ShotCount > 1)
		{
			float a = RandomHorizontalDirection();
			Rotation.Yaw += a;
			Player->AddControllerYawInput(a * 0.15f);
		}
	}


	Super::RecoilEndVec();
}


void AWRifle::SpawnShell()
{
	SpawnedShell = nullptr;
	BulletDecalBluePrint = nullptr;

	if (GetActorPool())
	{
		BulletDecalBluePrint = GetActorPool()->Get762Shell();
		SpawnedShell = GetWorld()->SpawnActor<AStaticMeshActor>(GetActorPool()->Get762Shell());
	}

	Super::SpawnShell();
}