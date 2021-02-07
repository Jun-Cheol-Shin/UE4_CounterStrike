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


	RecoilWeight = 1.5f;
	RandomRecoil = 0.f;

	RandomHorizontalRecoil = 0.f;
	RecoilHorizontalWeight = 1.0f;
	Direction = true;
	WeightSquare = false;
}

void AWRifle::Reload()
{
	if (AmmoCount == 0)
		return;

	Super::Reload();

	RecoilWeight = 1.5f;
	RandomRecoil = 0.f;

	RandomHorizontalRecoil = 0.f;
	RecoilHorizontalWeight = 1.5f;
	Direction = false;
	WeightSquare = false;
}

void AWRifle::ChangeRecoilDirection()
{
	// go right;
	if (ShotCount > 21)
	{
		Direction = false;
	}

	else if (ShotCount > 14)
	{
		Direction = true;
	}

	else if (ShotCount > 7)
	{
		Direction = false;
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
			RandomHorizontalRecoil += FMath::RandRange(HorizontalRandomValue, HorizontalRandomValue * 2.f);
		}

		else if(Direction && RandomHorizontalRecoil > -RealHitImpackHorizontalLimit)
		{
			RandomHorizontalRecoil -= FMath::RandRange(HorizontalRandomValue, HorizontalRandomValue * 2.f);
		}

		retval = RandomHorizontalRecoil;
	}

	return retval;
}

void AWRifle::RecoilEndVec()
{

	if(!RandomRecoilFlag)
	{
		switch (ShotCount)
		{
		case 0:
			break;
		case 1:
		case 2:
		case 3:
			RandomRecoil = FMath::RandRange(1.f, 1.15f);
			Rotation.Pitch += RandomRecoil;
			RandomRecoil *= RecoilWeight;
			break;

		default:
			if (RandomRecoil < RealHitImpactLimit && !WeightSquare)
			{
				RandomRecoil *= RecoilWeight;
				RecoilWeight += .1f;

				if (RandomRecoil > RealHitImpactLimit)
				{
					RandomRecoil = RealHitImpactLimit;
				}
			}

			else
			{
				WeightSquare = true;
				RandomRecoil = FMath::RandRange(RandomRecoil - 0.35f, RandomRecoil + 0.35f);
			}


			if (Player->GetFPSCharacterMovement()->IsCrouchHeld)
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
		}
	}


	Super::RecoilEndVec();
}


void AWRifle::SpawnShell()
{
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	AStaticMeshActor* Shell = nullptr;

	if (ActorPool)
	{
		Shell = ActorPool->GetMesh(EShellPoolList::ESP_762);
		if (Shell)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Shell Spawn!"));

			Shell->SetActorLocation(Player->GetCurrentFPSMesh()->GetSocketLocation(ShellSocketName));
			Shell->GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
			Shell->SetActorHiddenInGame(false);

			Shell->GetStaticMeshComponent()->AddImpulse(Player->FPSCameraComponent->GetRightVector() * 60.f);
		}
	}
}