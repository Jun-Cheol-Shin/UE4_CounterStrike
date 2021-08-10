// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "BulletShellActor.generated.h"



class USoundBase;

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API ABulletShellActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		USoundBase* ShellDropSound;

	ABulletShellActor();

protected:
	bool IsPlaying = false;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
