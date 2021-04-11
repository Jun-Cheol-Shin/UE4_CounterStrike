// Fill out your copyright notice in the Description page of Project Settings.


#include "WHandGun.h"
#include "UObject/NameTypes.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"

AWHandGun::AWHandGun()
{
	eGunNumber = EGunNumber::ONEHANDED;
	eWeaponNum = EWeaponNum::E_Sub;

	bCanAutoFire = false;

	SocketName = TEXT("OneHanded_Socket");

	RunSpeedRatio = 0.925f;

	PenatrateDecreaseDistanceRatio = 5.f;
}

void AWHandGun::ShakingCamera()
{
	if (CamShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), ShakingValue);
	}
}

void AWHandGun::RecoilEndVec()
{
	//float recoil = (float)ShotCount * 0.1f;

	//UE_LOG(LogTemp, Warning, TEXT("%.1f"), recoil);

	Rotation.Pitch += FMath::RandRange(-(float)ShotCount * 0.3f, (float)ShotCount * 0.3f);
	Rotation.Yaw += FMath::RandRange(-(float)ShotCount * 0.3f, (float)ShotCount * 0.3f);
}


void AWHandGun::ShuffleShotAnim()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		CurrentPlayingAnim = EmptyShotAnim;
	}

	else
	{
		uint8 random = FMath::RandRange(0, 2);

		switch (random)
		{
		case 1:
			CurrentPlayingAnim = AttackAnim_2;
			break;
		default:
			CurrentPlayingAnim = AttackAnim;
			break;
		}
		ShotCount += ShotRecoilStrength;
	}
	Super::ShuffleShotAnim();
}


void AWHandGun::SpawnShell()
{
	SpawnedShell = nullptr;
	BulletDecalBluePrint = nullptr;

	if (GetActorPool())
	{
		BulletDecalBluePrint = GetActorPool()->GetPistolShell();
		SpawnedShell = GetWorld()->SpawnActor<AStaticMeshActor>(GetActorPool()->GetPistolShell());
	}

	Super::SpawnShell();
}