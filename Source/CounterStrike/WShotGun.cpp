// Fill out your copyright notice in the Description page of Project Settings.


#include "WShotGun.h"
#include "DrawDebugHelpers.h"
#include "FPSCharacterStatComponent.h"
#include "ActorPool.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"

AWShotGun::AWShotGun()
{
	eWeaponNum = EWeaponNum::E_Rifle;
	eGunNumber = EGunNumber::SHOTGUN;
	bCanAutoFire = false;	
	TakeWeaponRunSpeed = 610.f;

	BackSocketName = "NOVA_Back";
}


void AWShotGun::AmmoReload()
{
	++CurrentOneMagazineAmmoCount;
	ElseAmmoCount = AmmoCount - CurrentOneMagazineAmmoCount;

	Player->GetFPSUIWidget()->SetAmmoCount(Player);
}

void AWShotGun::Reload()
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

		Player->DelayTime = ReloadAnim->SequenceLength + (InsertShell->SequenceLength * (OneMagazineAmmoCount - CurrentOneMagazineAmmoCount));

		CurrentPlayingAnim = ReloadAnim;
		PlayAnim(false);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			CurrentPlayingAnim = InsertShell;
			PlayAnim(true);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

				if (CurrentOneMagazineAmmoCount == OneMagazineAmmoCount)
				{
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
					CurrentPlayingAnim = AfterReload;
					PlayAnim(false);
					Idle();

					Player->bIsReloading = false;

					return;
				}

				if (Player->IsAttackHeld)
				{
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
					Player->bIsReloading = false;
					Fire();
					return;
				}

				AmmoReload();

			}, InsertShell->SequenceLength + 0.1f, true, 0.f);


		}, ReloadAnim->SequenceLength, false);
	}
}

void AWShotGun::GunShotMethod()
{
	Player->GetController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player);

	ShakingCamera();
	if (Player->GetFPSCharacterMovement()->Velocity.Size() > Player->GetFPSCharacterMovement()->CrouchSpeed)
	{
		CharacterVelocityToEffectAim();
	}

	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Player->GetCurrentFPSMesh(), MuzzleSocketName);
	SpawnShell();

	for (int i = 0; i < ShellBBCount; ++i)
	{
		FHitResult Hit;

		Rotation.Yaw += FMath::FRandRange(-i * 0.3f, i * 0.3f);
		Rotation.Pitch += FMath::FRandRange(-i * 0.3f, i * 0.3f);

		End = Location + Rotation.Vector() * Weapondistance;
		bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
			Location, End,
			ECollisionChannel::ECC_Visibility, CollisionParams);

		UNiagaraComponent* Particle =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTracer, Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName));

		FVector ModifyEnd = End - Player->GetCurrentFPSMesh()->GetSocketLocation(MuzzleSocketName);

		Particle->SetVectorParameter("BeamEnd", ModifyEnd);

		if (bSucess)
		{
			if (Hit.GetActor())
			{
				float anotherDistance;

				if (Hit.GetActor()->IsA(AFPSCharacter::StaticClass()))
				{
					AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());
					if (ActorPool)
					{
						DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, GunDamage,
							GunPenetration, DamagedCharacter->CheckHit(Hit.ImpactPoint.Z - Hit.GetActor()->GetActorLocation().Z), Hit.ImpactPoint, ActorPool->GetBloodParticle(), (End - Location).GetSafeNormal());
						SpawnDecal(Hit, EDecalPoolList::EDP_BLOOD);
					}
					anotherDistance = Weapondistance - FVector::Dist(Location, Hit.ImpactPoint) * 10.f;
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
}

void AWShotGun::ShakingCamera()
{
	if (CamShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), ShakingValue);
	}
}

void AWShotGun::ShuffleShotAnim()
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

	ShotCount += 60.f;
	Super::ShuffleShotAnim();
}


void AWShotGun::SpawnShell()
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
		Shell = ActorPool->GetMesh(EShellPoolList::ESP_SHOTGUN);
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