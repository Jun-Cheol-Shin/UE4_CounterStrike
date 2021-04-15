// Fill out your copyright notice in the Description page of Project Settings.


#include "WGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "ActorPool.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "FPSCharacterStatComponent.h"

AWGrenade::AWGrenade()
{
	eGunNumber = EGunNumber::ETC_GRENADE;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(RootComponent);
}

float AWGrenade::GetDistance(AFPSCharacter* DamagedActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("%.1f"), 98.f - FVector::Dist(DamagedActor->GetActorLocation(), GetActorLocation()) * 0.1f);
	return 98.f - FVector::Dist(DamagedActor->GetActorLocation(), GetActorLocation()) * 0.1f;
}


void AWGrenade::Explosion()
{
	Super::Explosion();

	RadialForce->FireImpulse();
	SpawnDecal();

	// need Damage...
	TArray<FHitResult> OutHits;

	// start and end locations
	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = GetActorLocation();

	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(RadialForce->Radius);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, MyColSphere);

	if (isHit)
	{
		// loop through TArray
		for (auto& Hit : OutHits)
		{
			if (Hit.GetActor())
			{
				if (Hit.GetActor()->IsA(AFPSCharacter::StaticClass()))
				{
					AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Hit.GetActor());

					if (ActorPool)
					{
						DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(GetDistance(DamagedCharacter), 1.f, Player, EBoneHit::EB_NONE, WeaponComponent->GetRelativeLocation());
					}
				}
			}
		}
	}

}