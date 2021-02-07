// Fill out your copyright notice in the Description page of Project Settings.


#include "WThrow.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacterStatComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "ActorPool.h"

AWThrow::AWThrow()
{
	eGunNumber = EGunNumber::ETC_GRENADE;
	TakeWeaponRunSpeed = 700.f;

	//ExplosionParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEffect"));
	//ExplosionParticle->SetupAttachment(RootComponent);
}

void AWThrow::ResetTimeHandle()
{
	Super::ResetTimeHandle();
}

void AWThrow::StopFire()
{
	Super::StopFire();
	if (bIsPossibleThrow)
	{
		bIsPossibleThrow = false;
		Throw();
	}
}

void AWThrow::Explosion()
{
	// play Sound..
	UGameplayStatics::SpawnSoundAttached(ExplodeSoundQueue, WeaponComponent);
	UGameplayStatics::SpawnEmitterAttached(ExplosionParticle, WeaponComponent, TEXT(""));
}


void AWThrow::Fire()
{
	if (Player)
	{
		CurrentPlayingAnim = AttackAnim;
		PlayAnim(false);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			if(!Player->IsAttackHeld)
			{
				Throw();
			}

			else
			{
				bIsPossibleThrow = true;
			}

		}, AttackAnim->SequenceLength, false);
	}
}

void AWThrow::ThrowProjectile()
{
	if (Player)
	{
		float arcValue;
		FRotator Rotation;
		FVector startLoc = FVector::ZeroVector;      // 발사 지점
		FVector outVelocity = FVector::ZeroVector;   // 결과 Velocity

		Player->GetController()->GetPlayerViewPoint(OUT startLoc, OUT Rotation);

		FVector targetLoc = startLoc + Rotation.Vector() * Weapondistance;  // 타겟 지점.


		if (Player->AimOffsetPitch > 45.f)
		{
			arcValue = 0.5f;
		}

		else
		{
			arcValue = 0.75f;
		}

		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
		{
			//FPredictProjectilePathParams predictParams(10.0f, startLoc, outVelocity, 5.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
			//predictParams.DrawDebugTime = 5.0f;     //디버그 라인 보여지는 시간 (초)
			//predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
			//predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
			//FPredictProjectilePathResult result;
			//UGameplayStatics::PredictProjectilePath(this, predictParams, result);

			FVector Location;
			Player->GetController()->GetPlayerViewPoint(Location, Rotation);
			Location += Player->FPSCameraComponent->GetForwardVector() * 100.f;

			this->SetActorLocation(Location);
			this->SetActorHiddenInGame(false);

			WeaponComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			WeaponComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			WeaponComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

			WeaponComponent->AddImpulse(outVelocity);

			//EWeaponNum Num = Player->DropAndEnableChangeWeapon();

			//if (uint8(Num) > 0)
			//{
			//	Player->GetFPSCharacterStatComponent()->RemoveSelectWeapon(Num);
			//}
		}

		/*else
		{
			WeaponComponent->AddImpulse(Player->FPSCameraComponent->GetForwardVector() *
				500.f * Player->GetFPSCharacterStatComponent()->GetSelectWeapon(
					Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber())->WeaponComponent->GetMass() + Player->FPSCameraComponent->GetUpVector() * 100.f);
		}*/
		
	}

}


void AWThrow::Throw()
{
	CurrentPlayingAnim = ThrowAnim;
	PlayAnim(false);

	Player->AttackAnimCall = true;
	Player->SetUpperState(UPPER_STATE::ATTACK);

	Player->DelayTime = CurrentPlayingAnim->SequenceLength;

	WeaponComponent->SetSimulatePhysics(true);

	ThrowProjectile();
	Player->ChangeWeapon(EWeaponNum::E_Knife);

	GetWorld()->GetTimerManager().SetTimer(ExplodeTimer, [this]() {

		GetWorld()->GetTimerManager().ClearTimer(ExplodeTimer);
		Explosion();

	}, ExplodeTime, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

		Player->AttackAnimCall = false;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		//WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
		//Idle();

	}, ThrowAnim->SequenceLength, false);

	if(!bIsInfinityGrenade)
	{ 
		Player->GetFPSCharacterStatComponent()->RemoveSelectWeapon(eWeaponNum);
	}
}


void AWThrow::SpawnDecal()
{
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	ADecalActor* ExplosionDecal = nullptr;

	if (ActorPool)
	{
		ExplosionDecal = ActorPool->GetDecal(EDecalPoolList::EDP_EXPLOSION);
		if (ExplosionDecal)
		{
			ExplosionDecal->SetActorLocation(WeaponComponent->GetRelativeLocation());
			ExplosionDecal->SetLifeSpan(5.0f);
			ExplosionDecal->GetDecal()->DecalSize = FVector(64.0f, 128.0f, 128.0f);
			ExplosionDecal->SetActorHiddenInGame(false);
		}
	}
}