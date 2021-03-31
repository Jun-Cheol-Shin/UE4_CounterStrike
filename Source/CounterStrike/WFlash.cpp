// Fill out your copyright notice in the Description page of Project Settings.


#include "WFlash.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "ActorPool.h"

AWFlash::AWFlash()
{
	eWeaponNum = EWeaponNum::E_Flash;
}

void AWFlash::Explosion()
{
	Super::Explosion();

	SpawnDecal();

	// need Decal Spawn..
	FlashCheck();

}



void AWFlash::FlashCheck()
{

	if (Player)
	{
		FVector DirectionCamToGrenade;

		DirectionCamToGrenade = this->GetActorLocation() - Player->FPSCameraComponent->GetComponentLocation();
		DirectionCamToGrenade.Normalize();

		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSCharacter::StaticClass(), actors);

		UE_LOG(LogTemp, Warning, TEXT("%d"), actors.Num());

		for (int i = 0; i < actors.Num(); ++i)
		{
			AFPSCharacter* Character = Cast<AFPSCharacter>(actors[i]);
			if (Character)
			{
				float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Character->FPSCameraComponent->GetForwardVector(), DirectionCamToGrenade)));
				if (Angle <= 90.f)
				{
					UE_LOG(LogTemp, Warning, TEXT(" %1.f FlashBang!"), Angle);
					Player->bIsFlashBang = true;
					UGameplayStatics::SpawnSoundAttached(BangSound, Player->GetMesh());
				}

				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%1.f Avoid!"), Angle);
				}
			}
		}
	}
}