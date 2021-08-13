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
		Player->SyncClientSendFlashBang(BangSound, this->GetActorLocation());
	}
}