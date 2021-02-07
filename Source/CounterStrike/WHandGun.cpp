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
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	AStaticMeshActor* Shell = nullptr;

	if (ActorPool)
	{
		Shell = ActorPool->GetMesh(EShellPoolList::ESP_PISTOL);
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