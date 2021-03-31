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
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"
#include "Math/UnrealMathUtility.h"
#include "FPSHUDWidget.h"

AWGun::AWGun()
{
	Weapondistance = 10000.f;
	MuzzleSocketName = TEXT("Muzzle");
	ShellSocketName = TEXT("Shell");

	//CurrentAmmoCount = AmmoCount;

	//static ConstructorHelpers::FClassFinder<ADecalActor> AmmoItem(TEXT("Blueprint'/Game/BluePrint/Decal/BulletHole_1.BulletHole_1_C'"));
	//if (AmmoItem.Succeeded())
	//{
	//	BulletDecalBluePrint = AmmoItem.Class;
	//}
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
		//UGameplayStatics::SpawnSoundAttached(DryMuzzleSound, Player->GetCurrentFPSMesh(), SocketName);
		/*if (Player && Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
		{
			Player->SyncClientSpawnSound(DryMuzzleSound, Player->GetCurrentThirdMesh());
		}*/

		Player->SyncClientSpawnSound(DryMuzzleSound, Player->GetCurrentThirdMesh());
	}

	else
	{
		//UGameplayStatics::SpawnSoundAttached(MuzzleSound, Player->GetCurrentFPSMesh(), SocketName);
		//if (Player && Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
		//{
		//	Player->SyncClientSpawnSound(MuzzleSound, Player->GetCurrentThirdMesh());
		//}

		Player->SyncClientSpawnSound(MuzzleSound, Player->GetCurrentThirdMesh());
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
			Rotation.Pitch += Player->GetMovementComponent()->Velocity.Size() * 0.01f;
			Rotation.Yaw += Player->GetMovementComponent()->Velocity.Size() * 0.01f;
			break;
		case 1:
			Rotation.Pitch += Player->GetMovementComponent()->Velocity.Size() * 0.01f;
			Rotation.Yaw -= Player->GetMovementComponent()->Velocity.Size() * 0.01f;
			break;
		case 2:
			Rotation.Pitch += Player->GetMovementComponent()->Velocity.Size() * 0.01f;
			break;
		}
	}
}

void AWGun::RefreshAmmoCount()
{

	CurrentAmmoCount = AmmoCount;

	if (CurrentAmmoCount - OneMagazineAmmoCount <= 0)
	{
		CurrentOneMagazineAmmoCount = CurrentAmmoCount;
		//AmmoCount = 0;
		ElseAmmoCount = 0;
	}

	else
	{
		CurrentOneMagazineAmmoCount = OneMagazineAmmoCount;
		ElseAmmoCount = CurrentAmmoCount - CurrentOneMagazineAmmoCount;
		//AmmoCount -= CurrentOneMagazineAmmoCount;
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
				//FirstShotRot = FRotator::ZeroRotator;
				//FirstShotRotBackUp = FRotator::ZeroRotator;
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Reset First Shot Rot!!!"));
			}

		}, GetAttackDelay() + 0.1f, false);
	}
}

void AWGun::AmmoReload()
{
	if (CurrentAmmoCount - OneMagazineAmmoCount <= 0)
	{
		CurrentOneMagazineAmmoCount = CurrentAmmoCount;
		//AmmoCount = 0;
		ElseAmmoCount = 0;
	}

	else
	{
		uint8 ReloadAmmo = OneMagazineAmmoCount - CurrentOneMagazineAmmoCount;

		ElseAmmoCount = CurrentAmmoCount - ReloadAmmo;
		//AmmoCount -= ReloadAmmo;
		CurrentOneMagazineAmmoCount = OneMagazineAmmoCount;
	}

	Player->GetFPSUIWidget()->SetAmmoCount(Player);
}

void AWGun::SetAmmoCount(uint8 CurrentAmmo, uint8 ElseAmmo)
{
	this->CurrentAmmoCount = CurrentAmmo + ElseAmmo;

	this->CurrentOneMagazineAmmoCount = CurrentAmmo;
	this->ElseAmmoCount = ElseAmmo;
}


void AWGun::ShakingCamera()
{
	if (CamShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), ShotCount + 9.f);
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("No Camera"));
		SetCamShake(Player->GetFPSCameraAttack());

		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), ShotCount + 9.f);
	}
}

void AWGun::DecreaseAmmo()
{
	if (!InfinityAmmo)
	{
		--CurrentOneMagazineAmmoCount;
		--CurrentAmmoCount;
	}
}

void AWGun::GunShotMethod()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ResetRecoilTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(ResetRecoilTimer);
		RandomRecoilFlag = true;
	}


	Player->GetController()->GetPlayerViewPoint(OUT Location, OUT Rotation);


	if (ShotCount == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Start Shot!!!"));
		//FirstShotRot = Rotation;
		//FirstShotRotBackUp = FirstShotRot;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player);


	if (Player->GetMovementComponent()->Velocity.Size() > Player->GetFPSCharacterStatComponent()->CrouchSpeed)
	{
		CharacterVelocityToEffectAim();
	}

	RecoilEndVec();

	// need modify..
	ShakingCamera();

	FHitResult Hit;
	//End = Location + Rotation.Vector() * Weapondistance;
	End = Location + Rotation.Vector() * Weapondistance;

	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
		Location, End,
		ECollisionChannel::ECC_Visibility, CollisionParams);


	SpawnShell();
	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Player->GetCurrentFPSMesh(), MuzzleSocketName);

	if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
	{
		Player->SyncClientSpawnMuzzleEffect(MuzzleParticle, Player->GetCurrentThirdMesh(), MuzzleSocketName);
	}

	if (bSucess)
	{
		if (Hit.GetActor())
		{
			float Distance = 0;
			bool flag = false;

			AWBase* Hitweapon = Cast<AWBase>(Hit.GetActor());
			AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());

			if (DamagedCharacter)
			{
				DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(GunDamage,
					GunPenetration, Player, DamagedCharacter->CheckHit(*Hit.BoneName.ToString()), (End - Location).GetSafeNormal());
				SpawnDecal(Hit, EDecalPoolList::EDP_BLOOD);
				Distance = Weapondistance - FVector::Dist(Location, Hit.ImpactPoint) * PenatrateDecreaseDistanceRatio;

			}

			else if(!Hitweapon)
			{
				SpawnDecal(Hit, EDecalPoolList::EDP_BULLETHOLE);
				flag = CheckPenetrationShot(Hit, (End - Location).GetSafeNormal());
				Distance = Weapondistance - FVector::Dist(Location, Hit.ImpactPoint) * PenatrateDecreaseDistanceRatio;
			}

			DrawDebugLine(GetWorld(), Location, Hit.ImpactPoint, FColor::Red, false, 2, 0, 1);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Remaining Distance : %.1f"), Distance));

			if (Distance > 0 && !Hitweapon && flag)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Penetrate!!!!"));
				PenetrationShot(Hit, (End - Location).GetSafeNormal(), Distance);
			}

			else
			{
				SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), Hit.ImpactPoint - Location);
			}

		}
	}

	else
	{
		SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), End - Location);
	}
}

// overriding...
void AWGun::Fire()
{
	if (Player)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("Current Ammo Count = %0d"), CurrentAmmoCount));
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
	if (CurrentAmmoCount == 0 || CurrentOneMagazineAmmoCount == OneMagazineAmmoCount)
	{
		Idle();
		return;
	}

	if (Player)
	{
		Player->bIsReloading = true;

		if (Player->GetLocalRole() < ROLE_Authority)
		{
			Player->SyncClientReload(true);
		}

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

			if (Player->GetLocalRole() < ROLE_Authority)
			{
				Player->SyncClientReload(false);
			}

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
	if (GetActorPool())
	{
		//ADecalActor* Decal = nullptr;

		switch (Type)
		{
		case EDecalPoolList::EDP_BULLETHOLE:
			//Decal = ActorPool->GetDecal(Player, EDecalPoolList::EDP_BULLETHOLE);
			//Decal = GetWorld()->SpawnActor<ADecalActor>(GetActorPool()->GetBulletHoleDecal());

		/*	if (Decal)
			{
				Decal->SetActorLocationAndRotation(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				Decal->SetActorHiddenInGame(false);

				if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
				{
					Player->SyncClientSpawnDecal(GetActorPool()->GetBulletHoleDecal(), Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				}
			}*/
			
			Player->SyncClientSpawnDecal(GetActorPool()->GetBulletHoleDecal(), Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetActorPool()->GetImpact(), FTransform(Hit.ImpactPoint), true);

			Player->SyncClientSpawnHitEffect(GetActorPool()->GetImpact(), Hit.ImpactPoint);

			break;

		case EDecalPoolList::EDP_BLOOD:
			//Decal = ActorPool->GetDecal(Player, EDecalPoolList::EDP_BLOOD);
			//Decal = GetWorld()->SpawnActor<ADecalActor>(GetActorPool()->GetBloodDecal());
			FHitResult PenetrationResult = CheckWall(Hit, (End - Location).GetSafeNormal() * 200.f, true);

			if (PenetrationResult.GetActor())
			{
				Player->SyncClientSpawnDecal(GetActorPool()->GetBloodDecal(), PenetrationResult.ImpactPoint, PenetrationResult.ImpactNormal.Rotation());
			}

			else
			{
				PenetrationResult = CheckWall(Hit, -Hit.GetActor()->GetActorUpVector() * 1000.f, true);
				if (PenetrationResult.GetActor())
				{
					FVector Vec = FVector(FMath::RandRange(PenetrationResult.ImpactPoint.X - 50.f, PenetrationResult.ImpactPoint.X + 50.f),
						FMath::RandRange(PenetrationResult.ImpactPoint.Y - 50.f, PenetrationResult.ImpactPoint.Y + 50.f), PenetrationResult.ImpactPoint.Z);

					Player->SyncClientSpawnDecal(GetActorPool()->GetBloodDecal(), Vec + (End - Location).GetSafeNormal() * 100.f,
						PenetrationResult.ImpactNormal.Rotation());
				}
			}

			Player->SyncClientSpawnHitEffect(GetActorPool()->GetBlood(), Hit.ImpactPoint);
			/*if (Decal)
			{
				FHitResult PenetrationResult = CheckWall(Hit, (End - Location).GetSafeNormal() * 200.f, true);

				if (PenetrationResult.GetActor())
				{
					Decal->SetActorLocationAndRotation(PenetrationResult.ImpactPoint, PenetrationResult.ImpactNormal.Rotation());
					Decal->SetActorHiddenInGame(false);

					if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
					{
						Player->SyncClientSpawnDecal(GetActorPool()->GetBloodDecal(), PenetrationResult.ImpactPoint, PenetrationResult.ImpactNormal.Rotation());
					}

				}

				else
				{
					PenetrationResult = CheckWall(Hit, -Hit.GetActor()->GetActorUpVector() * 1000.f, true);
					if (PenetrationResult.GetActor())
					{
						FVector Vec = FVector(FMath::RandRange(PenetrationResult.ImpactPoint.X - 50.f, PenetrationResult.ImpactPoint.X + 50.f),
							FMath::RandRange(PenetrationResult.ImpactPoint.Y - 50.f, PenetrationResult.ImpactPoint.Y + 50.f), PenetrationResult.ImpactPoint.Z);

						Decal->SetActorLocationAndRotation(Vec + (End - Location).GetSafeNormal() * 100.f, PenetrationResult.ImpactNormal.Rotation());
						Decal->SetActorHiddenInGame(false);
						if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
						{
							Player->SyncClientSpawnDecal(GetActorPool()->GetBloodDecal(), Vec + (End - Location).GetSafeNormal() * 100.f,
								PenetrationResult.ImpactNormal.Rotation());
						}

					}
				}

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetActorPool()->GetBlood(), FTransform(Hit.ImpactPoint), true);
				if (Player && Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
				{
					Player->SyncClientSpawnHitEffect(GetActorPool()->GetBlood(), Hit.ImpactPoint);
				}
			}*/
			break;
		}
	}
}

void AWGun::PenetrationShot(FHitResult Point, FVector Direction, float Distance)
{
	FHitResult Hit;
	bool flag = false;
	bool bSuccess = false;
	float DecreaseRatio = 0.15;

	//float RemainDistance = Distance;
	//FHitResult AnotherPoint = Point;
	//uint8 PenetrateCount = 1;
	FCollisionQueryParams Param;
	Param.AddIgnoredActor(Player);
	//Param.AddIgnoredActor(AnotherPoint.GetActor());

	bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Point.ImpactPoint + Direction, Point.ImpactPoint + Direction * Distance,
		ECC_Visibility, Param);

	DrawDebugLine(GetWorld(), Point.ImpactPoint, Point.ImpactPoint + Direction * Distance, FColor::Blue, false, 2, 0, 1);

	if (Hit.GetActor())
	{
		if (Hit.GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Penetrate Character Hit!!!!"));
			AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());
			SpawnDecal(Hit, EDecalPoolList::EDP_BLOOD);
			DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(GunDamage - 3,
				GunPenetration - DecreaseRatio, Player, DamagedCharacter->CheckHit(*Hit.BoneName.ToString()));
			//RemainDistance -= FVector::Dist(AnotherPoint.ImpactPoint, Hit.ImpactPoint) * 10.f;
		}

		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Penetrate Wall Hit!!!!"));
			SpawnDecal(Hit, EDecalPoolList::EDP_BULLETHOLE);
			//flag = CheckPenetrationShot(Hit, Direction);
			//RemainDistance -= FVector::Dist(AnotherPoint.ImpactPoint, Hit.ImpactPoint) * 20.f;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Remaining Distance : %.1f"), Distance));
		//DrawDebugLine(GetWorld(), Point.ImpactPoint, Hit.ImpactPoint, FColor::Blue, false, 2, 0, 1);


		SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), Hit.ImpactPoint - Location);
		//AnotherPoint = Hit;
	}

	//SpawnNiagra(Point.ImpactPoint, Hit.ImpactPoint);

}


bool AWGun::CheckPenetrationShot(FHitResult Point, FVector Direction)
{
	float thickness = Point.GetActor()->GetActorScale3D().Size2D() * 150.f;
	FHitResult FinalHit;
	FVector Start = Point.ImpactPoint + Direction * thickness;

	bool bSuccess = false;

	bSuccess = GetWorld()->LineTraceSingleByChannel(FinalHit,
		Start, Start - Direction * thickness,
		ECollisionChannel::ECC_Visibility, FCollisionQueryParams());

	//DrawDebugLine(GetWorld(), Start, Start - Direction * thickness, FColor::Red, false, 2, 0, 1);

	if (bSuccess)
	{
		if (FinalHit.GetActor() && !FinalHit.GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			SpawnDecal(FinalHit, EDecalPoolList::EDP_BULLETHOLE);
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Good! Decal Spawn!"));

			return true;
		}

		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Decal Spawn?"));
		}

		
	}

	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Not Decal Spawn!"));
	}

	return false;
}


void AWGun::SpawnNiagra(FVector ParticleStart, FVector ParitcleEnd)
{
	UNiagaraComponent* Particle =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTracer, ParticleStart);

	//Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName)
	//FVector ModifyEnd = (AnotherPoint.ImpactPoint + Direction) - Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName);

	if (Particle)
	{
		Particle->SetVectorParameter("BeamEnd", ParitcleEnd);
	}


	if (ParticleStart.Equals(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName)))
	{
		Player->SyncClientSpawnEffect(BulletTracer, Player->GetCurrentThirdMesh(), ParitcleEnd);
	}

	//else
	//{
	//	Player->SyncSpawnPenetrateEffect(BulletTracer, ParticleStart, ParitcleEnd);
	//}
}


void AWGun::SpawnShell()
{
	if (SpawnedShell && BulletDecalBluePrint)
	{
		float Power = FMath::RandRange(120.f, 180.f);

		//AStaticMeshActor* Shell = nullptr;
		//Shell = GetWorld()->SpawnActor<AStaticMeshActor>(BulletDecalBluePrint);

		//UE_LOG(LogTemp, Warning, TEXT("Shell Spawn!"));
		SpawnedShell->SetActorRotation(FRotator(0, 90, 0));
		SpawnedShell->SetActorLocation(Player->GetCurrentFPSMesh()->GetSocketLocation(ShellSocketName));
		SpawnedShell->GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		SpawnedShell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		SpawnedShell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		SpawnedShell->SetActorHiddenInGame(false);
		SpawnedShell->GetStaticMeshComponent()->AddImpulse(Player->FPSCameraComponent->GetRightVector() * Power);

		if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
		{
			Player->SyncClientSpawnShell(BulletDecalBluePrint,
				Player->FPSCameraComponent->GetRightVector() * Power, Player->GetCurrentThirdMesh());
		}

		//Player->SyncClientSpawnShell(BulletDecalBluePrint,
		//	Player->FPSCameraComponent->GetRightVector() * Power, Player->GetCurrentThirdMesh());
	}
}

void AWGun::InitAmmoCount()
{
	SetAmmoCount(OneMagazineAmmoCount, AmmoCount - OneMagazineAmmoCount);
}
