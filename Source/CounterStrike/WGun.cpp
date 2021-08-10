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


	//if (ShotCount == 0)
	//{
	//	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Start Shot!!!"));
	//	//FirstShotRot = Rotation;
	//	//FirstShotRotBackUp = FirstShotRot;
	//}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player);


	if (Player->GetMovementComponent()->Velocity.Size() > Player->GetFPSCharacterStatComponent()->CrouchSpeed)
	{
		CharacterVelocityToEffectAim();
	}

	RecoilEndVec();

	// need modify..
	ShakingCamera();

	TArray<FHitResult> Hits;
	FHitResult Backup;
	End = Location + Rotation.Vector() * Weapondistance;

	FCollisionObjectQueryParams ObjectList;
	ObjectList.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectList.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	bool bSucess = GetWorld()->LineTraceMultiByObjectType(Hits, Location, End, ObjectList, CollisionParams);

		/*bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
			Location, End,
			ECollisionChannel::ECC_Visibility, CollisionParams);*/


	SpawnShell();

	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Player->GetCurrentFPSMesh(), MuzzleSocketName);
	Player->SyncClientSpawnMuzzleEffect(MuzzleParticle, Player->GetCurrentThirdMesh(), MuzzleSocketName);

	if (bSucess && Hits[0].GetActor())
	{
		AWBase* Hitweapon = Cast<AWBase>(Hits[0].GetActor());
		AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hits[0].GetActor());

		float Distance = Weapondistance;
		FVector dir = (End - Location).GetSafeNormal();

		// 총알을 쏜 장소에서 부딪힌 사물의 거리만큼 빼준다.
		Distance -= FVector::Dist(Location, Hits[0].ImpactPoint) * PenatrateDecreaseDistanceRatio;

		if (DamagedCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Hit : %s"), *(Hits[0].BoneName.ToString())));
			DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(GunDamage,
				GunPenetration, Player, DamagedCharacter->CheckHit(*(Hits[0].BoneName.ToString())), (End - Location).GetSafeNormal());
			SpawnDecal(Hits[0], EDecalPoolList::EDP_BLOOD);
		}

		else 
			SpawnDecal(Hits[0], EDecalPoolList::EDP_BULLETHOLE);

		TArray<FHitResult> pPoints;
		FHitResult pPoint = Hits[0];
		Backup = pPoint;

		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Hit Count : %d"), Hits.Num()));

		if (!Hitweapon)
		{
			// 현재 맞은 사물의 관통여부 확인
			pPoint = CheckPenetrationShot(Hits, dir);

			 //유효거리가 남았고, 관통여부가 확인된다면 다음 샷을 진행
			while (Distance > 0.f && pPoint.GetActor())
			{
				// HitResult 값 백업
				Backup = pPoint;
				// 다음 사물 맞은 곳을 리턴
				pPoints = PenetrationShot(pPoint, dir, Distance);

				// 맞았다면.. 관통여부 확인한다. 맞지 않았다면 브레이크
				if (pPoints.Num() == 0)
					break;
				else
					pPoint = CheckPenetrationShot(pPoints, dir);
			}
		}

		if(pPoint.GetActor())
			SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), pPoint.ImpactPoint - Location);
	
		else 
			SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), Backup.ImpactPoint - Location);
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

FHitResult AWGun::CheckPenetrationShot(const TArray<FHitResult>& Point, const FVector& Direction)
{
	FHitResult retval;
	bool bSuccess = false;

	FCollisionQueryParams params;
	params.AddIgnoredActor(Player);

	if (Point.Num() > 1)
	{
		for (int i = 1; i < Point.Num(); ++i)
		{
			params.AddIgnoredActor(Point[i].GetActor());
		}

		bSuccess = GetWorld()->LineTraceSingleByChannel(retval,
			Point[1].ImpactPoint, Point[0].ImpactPoint,
			ECollisionChannel::ECC_Visibility, params);
	}

	else
	{
		bSuccess = GetWorld()->LineTraceSingleByChannel(retval,
			Point[0].TraceEnd, Point[0].Location,
			ECollisionChannel::ECC_Visibility, params);
	}

	// 관통 성공.. 동시에 물체의 반대편에 데칼 생성 (내가 총알을 맞춘 액터와 동일 액터라면...)
	if (bSuccess && retval.GetActor())
	{
		AWBase* Hitweapon = Cast<AWBase>(retval.GetActor());
		if (Hitweapon) return FHitResult();

		if (retval.GetActor()->IsA(AFPSCharacter::StaticClass()))
			SpawnDecal(retval, EDecalPoolList::EDP_BLOOD);
		else 
			SpawnDecal(retval, EDecalPoolList::EDP_BULLETHOLE);
	}

	return retval;
}

TArray<FHitResult> AWGun::PenetrationShot(const FHitResult& Point, const FVector& Direction, float& Distance)
{
	// 관통에 성공했다면 실행되는 함수..


	TArray<FHitResult> Hits;
	//FHitResult Hit;
	bool bSuccess = false;
	float DecreaseRatio = 0.15;

	FCollisionQueryParams Param;
	Param.AddIgnoredActor(Player);
	Param.AddIgnoredActor(Point.GetActor());

	FCollisionObjectQueryParams ObjectList;
	ObjectList.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectList.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	bSuccess = GetWorld()->LineTraceMultiByObjectType(Hits, Point.ImpactPoint, Point.ImpactPoint + Direction * Distance,
		ObjectList, Param);


	if (bSuccess && Hits[0].GetActor())
	{
		// 캐릭터에 맞았다면..
		if (Hits[0].GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Penetrate Character Hit!!!!"));
			AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hits[0].GetActor());
			SpawnDecal(Hits[0], EDecalPoolList::EDP_BLOOD);

			// 무기마다 정해진 데미지, 방탄복 관통력을 감소시켜서 데미지를 적용시킨다.
			DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(GunDamage - 3,
				GunPenetration - DecreaseRatio, Player, DamagedCharacter->CheckHit(*(Hits[0].BoneName.ToString())));
		}

		// 그 외 물체에 맞았다면..
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Penetrate Wall Hit!!!!"));
			SpawnDecal(Hits[0], EDecalPoolList::EDP_BULLETHOLE);
		}

		// 나이아가라 이펙트 호출
		//SpawnNiagra(Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName), Hit.ImpactPoint - Location);
		Distance -= FVector::Dist(Point.ImpactPoint, Hits[0].ImpactPoint) * PenatrateDecreaseDistanceRatio;
		DrawDebugLine(GetWorld(), Point.ImpactPoint, Hits[0].ImpactPoint, FColor::Blue, false, 10, 0, 1);
	}

	else Distance -= Distance;
	
	return Hits;
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

		Player->SyncClientSpawnShell(BulletDecalBluePrint,
			Player->FPSCameraComponent->GetRightVector() * Power, Player->GetCurrentThirdMesh());

		//Player->SyncClientSpawnShell(BulletDecalBluePrint,
		//	Player->FPSCameraComponent->GetRightVector() * Power, Player->GetCurrentThirdMesh());
	}
}

void AWGun::InitAmmoCount()
{
	SetAmmoCount(OneMagazineAmmoCount, AmmoCount - OneMagazineAmmoCount);
}
