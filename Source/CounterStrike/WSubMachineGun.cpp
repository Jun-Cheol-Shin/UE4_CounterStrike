// Fill out your copyright notice in the Description page of Project Settings.


#include "WSubMachineGun.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"
#include "Kismet/GameplayStatics.h"

AWSubMachineGun::AWSubMachineGun()
{

	BackSocketName = "Mac10_Back";

	RunSpeedRatio = 0.91f;

	PenatrateDecreaseDistanceRatio = 25.f;
}

void AWSubMachineGun::ChangeRecoilDirection()
{
	Super::ChangeRecoilDirection();
}

void AWSubMachineGun::SpawnShell()
{
	SpawnedShell = nullptr;
	BulletDecalBluePrint = nullptr;

	if (GetActorPool())
	{
		BulletDecalBluePrint = GetActorPool()->GetRifleShell();
		SpawnedShell = GetWorld()->SpawnActor<AStaticMeshActor>(GetActorPool()->GetRifleShell());
	}

	Super::SpawnShell();
}