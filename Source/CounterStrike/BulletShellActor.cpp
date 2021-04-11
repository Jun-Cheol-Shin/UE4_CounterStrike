// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShellActor.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ABulletShellActor::ABulletShellActor()
{
	this->InitialLifeSpan = 5.f;
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("Shell"));
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMassOverrideInKg(TEXT(""), 1.f);
	GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
}



void ABulletShellActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsPlaying)
	{
		IsPlaying = true;
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShellDropSound, HitLocation);
	}
}
