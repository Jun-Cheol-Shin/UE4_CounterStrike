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
	dir = EC_Direction::RIGHT;
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
	dir = EC_Direction::RIGHT;
	//WeightSquare = false;
}

void AWRifle::ChangeRecoilDirection()
{
	switch(dir)
	{
	case EC_Direction::LEFT:
		dir = EC_Direction::RIGHT;
		break;
	case EC_Direction::RIGHT:
		dir = EC_Direction::LEFT;
		break;
	}

	/*GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Direction : %d"), dir));*/
	
}



void AWRifle::RandomHorizontalDirection()
{
	float retval = 0.f;

	// MouseFocusingLimit을 넘기 전 까지 랜덤으로 수평반동이 튄다.
	if (RandomRecoil < MouseFocusingLimit)
	{
		retval = FMath::RandRange(-1.f, 1.f);
		Rotation.Yaw += retval;
		// 카메라의 위치 값 조정
		Player->AddControllerYawInput(retval * 0.15f);
	}

	else
	{
		// 방향에 따라 수평으로 반동이 튀도록 함.
		switch(dir)
		{
		case EC_Direction::MIDDLE:
			break;
		case EC_Direction::LEFT:
			RandomHorizontalRecoil -= FMath::RandRange(HorizontalRandomValue * 1.5f,
				HorizontalRandomValue * 1.7f);
			Player->AddControllerYawInput(RandomHorizontalRecoil * 0.15f);
			break;
		case EC_Direction::RIGHT:
			// 카메라의 위치 값 조정
			RandomHorizontalRecoil += FMath::RandRange(HorizontalRandomValue * 1.3f,
				HorizontalRandomValue * 1.5f);
			Player->AddControllerYawInput(RandomHorizontalRecoil * 0.15f);
			break;
		}
		
		Rotation.Yaw += RandomHorizontalRecoil;
	}

	// 방향 바꾸기
	if(RandomHorizontalRecoil > RealHitImpackHorizontalLimit && dir == EC_Direction::RIGHT)
	{
		// 단순 dir 변수 바꾸기 함수
		ChangeRecoilDirection();
	}

	else if(RandomHorizontalRecoil < -RealHitImpackHorizontalLimit * 0.8f && dir == EC_Direction::LEFT)
	{
		ChangeRecoilDirection();
	}
}

void AWRifle::RecoilEndVec()
{
	if(!RandomRecoilFlag)
	{
		// 수평반동의 방향을 결정하는 함수
		RandomHorizontalDirection();
		
		switch (ShotCount)
		{
			// 3발까지 수직반동이 랜덤하게 튀도록 설정.
		case 0:
			Player->AddControllerPitchInput(-RandomRecoil * 0.1f);
			break;
		case 1:
		case 2:
		case 3:
			RandomRecoil = FMath::RandRange(0.8f, 1.15f);
			Rotation.Pitch += RandomRecoil;
			// 가중치 더하기
			RandomRecoil += RecoilWeight;
			Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
			break;

		default:
			// 그 이후부터 정해진 방향에 따라 일정하게 (RecoilWeight만큼) 수직반동이 튀도록 설정
			if(RandomRecoil < RealHitImpactLimit)
			{
				// 반동 값에 가중치를 더한다
				RandomRecoil += RecoilWeight;
				// 가중치가 증가
				RecoilWeight += .1f;
				Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
			}
			
			// 카메라의 위치 값 조정
			if (Player->IsCrouchHeld)
			{
				RandomRecoil *= 0.6f;
			}
			
			Rotation.Pitch += RandomRecoil;
			//Player->AddControllerPitchInput(-RandomRecoil * 0.15f);
			break;
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