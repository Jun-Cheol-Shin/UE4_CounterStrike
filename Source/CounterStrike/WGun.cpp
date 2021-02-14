// Fill out your copyright notice in the Description page of Project Settings.

#include "WGun.h"

#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "FPSCameraShake.h"
#include "FPSCharacterStatComponent.h"
#include "WM4A1.h"
#include "WUSP.h"
#include "ActorPool.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"

AWGun::AWGun()
{
	Weapondistance = 5000.f;
	MuzzleSocketName = TEXT("Muzzle");
	ShellSocketName = TEXT("Shell");
}

void AWGun::RecoilEndVec()
{
	if (RandomRecoilFlag)
	{
		float YawValue = FMath::RandRange(-0.6f, 0.6f);
		Rotation.Pitch += ShotCount * 0.6f;
		Rotation.Yaw += YawValue;

		RandomRecoilFlag = false;
	}
}

void AWGun::ResetTimeHandle()
{
	GetWorldTimerManager().ClearTimer(ResetRecoilTimer);
	Super::ResetTimeHandle();
}

void AWGun::PlayShotSound()
{
	if (CurrentOneMagazineAmmoCount == 0)
	{
		UGameplayStatics::SpawnSoundAttached(DryMuzzleSound, Player->GetMesh(), SocketName);
	}

	else
	{
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, Player->GetMesh(), SocketName);
	}
}


void AWGun::CharacterVelocityToEffectAim()
{
	if (Player)
	{
		uint8 Direction = FMath::RandRange(0, 3);

		switch (Direction)
		{
		case 0:
			Rotation.Pitch += Player->GetFPSCharacterMovement()->Velocity.Size() * 0.01f;
			Rotation.Yaw += Player->GetFPSCharacterMovement()->Velocity.Size() * 0.01f;

			break;
		case 1:
			Rotation.Pitch += Player->GetFPSCharacterMovement()->Velocity.Size() * 0.01f;
			Rotation.Yaw -= Player->GetFPSCharacterMovement()->Velocity.Size() * 0.01f;
			break;
		case 2:
			Rotation.Pitch += Player->GetFPSCharacterMovement()->Velocity.Size() * 0.01f;
			break;
		}

	}
}

void AWGun::RefreshAmmoCount()
{
	if (AmmoCount - OneMagazineAmmoCount <= 0)
	{
		CurrentOneMagazineAmmoCount = AmmoCount;
		AmmoCount = 0;
		ElseAmmoCount = 0;
	}

	else
	{
		CurrentOneMagazineAmmoCount = OneMagazineAmmoCount;
		ElseAmmoCount = AmmoCount - CurrentOneMagazineAmmoCount;
		AmmoCount -= CurrentOneMagazineAmmoCount;
	}
}

void AWGun::SetCamShake(TSubclassOf<UCameraShake> CamComp)
{

	CamShake = Cast<UFPSCameraShake>(CamComp.GetDefaultObject());

	//if (CamShake)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Get CamShake!"));
	//}

	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Not Found CamShake!"));
	//}
}

void AWGun::StopFire()
{
	Super::StopFire();

	if (!GetWorld()->GetTimerManager().IsTimerActive(ResetRecoilTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(ResetRecoilTimer, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(ResetRecoilTimer);
			if (ShotCount > 0)
			{
				ShotCount = 0;
			}

		}, GetAttackDelay() + 0.2f, false);
	}
}

void AWGun::AmmoReload()
{
	if (AmmoCount - OneMagazineAmmoCount <= 0)
	{
		CurrentOneMagazineAmmoCount = AmmoCount;
		AmmoCount = 0;
		ElseAmmoCount = 0;
	}

	else
	{
		uint8 ReloadAmmo = OneMagazineAmmoCount - CurrentOneMagazineAmmoCount;

		ElseAmmoCount = AmmoCount - ReloadAmmo;
		AmmoCount -= ReloadAmmo;
		CurrentOneMagazineAmmoCount = OneMagazineAmmoCount;
	}

	Player->GetFPSUIWidget()->SetAmmoCount(Player);
}

void AWGun::ShakingCamera()
{
	if (CamShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), ShotCount + 9.f);
	}
}

void AWGun::DecreaseAmmo()
{
	--CurrentOneMagazineAmmoCount;
}

void AWGun::GunShotMethod()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ResetRecoilTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(ResetRecoilTimer);
		RandomRecoilFlag = true;
	}


	if (Player->GetisFirstView())
	{
		SpawnShell();
		UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Player->GetCurrentFPSMesh(), MuzzleSocketName);
	}

	Player->GetController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player);




	if (Player->GetFPSCharacterMovement()->Velocity.Size() > Player->GetFPSCharacterMovement()->CrouchSpeed)
	{
		CharacterVelocityToEffectAim();
	}
	RecoilEndVec();

	// need modify..
	ShakingCamera();

	FHitResult Hit;
	End = Location + Rotation.Vector() * Weapondistance;
	//UE_LOG(LogTemp, Warning, TEXT("%.1f , %.1f"), RandomRecoil, RandomHorizontalRecoil);

	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
		Location, End,
		ECollisionChannel::ECC_Visibility, CollisionParams);

	if (Player->GetisFirstView())
	{
		UNiagaraComponent* Particle =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTracer, Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName));

		FVector ModifyEnd = End - Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName);

		Particle->SetVectorParameter("BeamEnd", ModifyEnd);
	}

	if (bSucess)
	{
		if (Hit.GetActor())
		{
			float anotherDistance = 0;

			AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());
			if (DamagedCharacter)
			{	
				//UE_LOG(LogTemp, Warning, TEXT("%.1f"), Hit.ImpactPoint.Z - Hit.GetActor()->GetActorLocation().Z);
				//AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());
				if (ActorPool)
				{
					DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, GunDamage,
						GunPenetration, DamagedCharacter->CheckHit(Hit.ImpactPoint.Z - Hit.GetActor()->GetActorLocation().Z), Hit.ImpactPoint, Player, (End - Location).GetSafeNormal());
					SpawnDecal(Hit, EDecalPoolList::EDP_BLOOD);
					anotherDistance = Weapondistance - FVector::Dist(Location, Hit.ImpactPoint) * 10.f;
				}
			}

			else
			{
				SpawnDecal(Hit, EDecalPoolList::EDP_BULLETHOLE);
				CheckPenetrationShot(Hit, (End - Location).GetSafeNormal());
				anotherDistance = Weapondistance - FVector::Dist(Location, Hit.ImpactPoint) * 20.f;
			}		

			if (anotherDistance > 0)
			{
				PenetrationShot(Hit, (End - Location).GetSafeNormal(), anotherDistance);
			}

		}
	}
}

// overriding...
void AWGun::Fire()
{
	if (Player)
	{
		PlayShotSound();

		if (CurrentOneMagazineAmmoCount == 0)
		{
			Idle();
			return;
		}
		DecreaseAmmo();
		GunShotMethod();
		Player->GetFPSUIWidget()->SetAmmoCount(Player);

		Super::Fire();
	}
}

void AWGun::Action()
{
	if (Player)
	{
		if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
		{
			if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->IsA(AWM4A1::StaticClass()) ||
				Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->IsA(AWUSP::StaticClass()))
			{
				Super::Action();
			}
		}
	}
}

void AWGun::Reload()
{
	if (AmmoCount == 0 || CurrentOneMagazineAmmoCount == OneMagazineAmmoCount)
	{
		Idle();
		return;
	}

	if (Player)
	{
		Player->bIsReloading = true;

		ShotCount = 0;
		Player->SetUpperState(UPPER_STATE::RELOAD);
		Player->DelayTime = ReloadAnim->SequenceLength;

		CurrentPlayingAnim = ReloadAnim;
		PlayAnim(false);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon() != this)
			{
				return;
			}

			Player->SetUpperState(UPPER_STATE::IDLE);
			Player->bIsReloading = false;
			AmmoReload();
			Player->ReloadStartTime = 0.f;

		}, ReloadAnim->SequenceLength, false);
	}
}


void AWGun::BeginPlay()
{
	Super::BeginPlay();
	RefreshAmmoCount();
}


void AWGun::SpawnDecal(FHitResult Hit, EDecalPoolList Type)
{
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	ADecalActor* Decal = nullptr;

	if (ActorPool)
	{
		switch (Type)
		{
		case EDecalPoolList::EDP_BULLETHOLE:
			Decal = ActorPool->GetDecal(EDecalPoolList::EDP_BULLETHOLE);
			if (Decal)
			{
				Decal->SetActorLocation(Hit.ImpactPoint);
				Decal->SetActorRotation(Hit.ImpactNormal.Rotation());
				Decal->SetActorHiddenInGame(false);
			}
			break;
		case EDecalPoolList::EDP_BLOOD:
			Decal = ActorPool->GetDecal(EDecalPoolList::EDP_BLOOD);
			if (Decal)
			{
				FHitResult PenetrationResult = CheckWall(Hit, (End - Location).GetSafeNormal() * 200.f, true);

				if (PenetrationResult.GetActor())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Penetration!!! %s "), *PenetrationResult.GetActor()->GetName());

					Decal->SetActorLocation(PenetrationResult.ImpactPoint);
					Decal->SetActorRotation(PenetrationResult.ImpactNormal.Rotation());
					Decal->SetActorHiddenInGame(false);

				}

				else
				{
					PenetrationResult = CheckWall(Hit, -Hit.GetActor()->GetActorUpVector() * 1000.f, true);
					if (PenetrationResult.GetActor())
					{
						FVector Vec = FVector(FMath::RandRange(PenetrationResult.ImpactPoint.X - 50.f, PenetrationResult.ImpactPoint.X + 50.f),
							FMath::RandRange(PenetrationResult.ImpactPoint.Y - 50.f, PenetrationResult.ImpactPoint.Y + 50.f), PenetrationResult.ImpactPoint.Z);
						Decal->SetActorLocation(Vec + (End - Location).GetSafeNormal() * 100.f);
						Decal->SetActorRotation(PenetrationResult.ImpactNormal.Rotation());
						Decal->SetActorHiddenInGame(false);
					}
				}
			}
			break;
		}
	}
}

void AWGun::PenetrationShot(FHitResult Point, FVector Direction, float Distance)
{
	bool bSuccess = false;

	float RemainDistance = Distance;
	FHitResult AnotherPoint = Point;

	uint8 PenetrateCount = 1;
	float DecreaseRatio = 0.1;

	do 
	{
		bool flag = false;
		FHitResult Hit;
		FCollisionQueryParams Param;
		Param.AddIgnoredActor(Player);
		Param.AddIgnoredActor(AnotherPoint.GetActor());
		bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, AnotherPoint.ImpactPoint, AnotherPoint.ImpactPoint + Direction * RemainDistance,
			ECC_Visibility, Param);

		if (bSuccess)
		{
			if (Hit.GetActor())
			{
				if (Hit.GetActor()->IsA(AFPSCharacter::StaticClass()))
				{
					AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());

					SpawnDecal(Hit, EDecalPoolList::EDP_BLOOD);
					DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, GunDamage - 3 * PenetrateCount,
						GunPenetration - DecreaseRatio * PenetrateCount, DamagedCharacter->CheckHit(Hit.ImpactPoint.Z - Hit.GetActor()->GetActorLocation().Z), Hit.ImpactPoint, Player);
					RemainDistance -= FVector::Dist(AnotherPoint.ImpactPoint, Hit.ImpactPoint) * 10.f;
				}

				else
				{
					SpawnDecal(Hit, EDecalPoolList::EDP_BULLETHOLE);
					CheckPenetrationShot(Hit, Direction);
					RemainDistance -= FVector::Dist(AnotherPoint.ImpactPoint, Hit.ImpactPoint) * 20.f;
				}
			}

			else
			{
				break;
			}

			AnotherPoint = Hit;
		}

		else
		{
			break;
		}

		if (RemainDistance < 0) break;

	} while(bSuccess);
}


void AWGun::CheckPenetrationShot(FHitResult Point, FVector Direction)
{
	float thickness = Point.GetActor()->GetActorScale3D().Size2D() * 100.f;
	FHitResult FinalHit;
	FVector Start = Point.ImpactPoint + Direction * thickness;

	bool bSuccess;
	bSuccess = GetWorld()->LineTraceSingleByChannel(FinalHit,
		Start, Start - Direction * thickness,
		ECollisionChannel::ECC_Visibility, FCollisionQueryParams());

	if (bSuccess)
	{
		if (FinalHit.GetActor() && !FinalHit.GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Success!!"));
			SpawnDecal(FinalHit, EDecalPoolList::EDP_BULLETHOLE);
		}
	}

}

