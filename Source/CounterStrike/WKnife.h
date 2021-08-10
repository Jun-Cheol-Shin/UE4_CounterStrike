// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WBase.h"

#include "Engine/EngineTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

#include "WKnife.generated.h"

/**
 * 
 */

class AActorPool;

UCLASS()
class COUNTERSTRIKE_API AWKnife : public AWBase
{
	GENERATED_BODY()

private:
	
	//TArray<FVector> KnifeAttackDirection;
	//TArray<AActor*> HitResults;
	TArray<FHitResult> HitPoint;

public:
	virtual ~AWKnife()
	{

	}
	AWKnife();
	UPROPERTY(EditAnywhere)
		float Penetration;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim_2;
	UPROPERTY(EditAnywhere)
		UAnimSequence* ActionHitAnim;
	UPROPERTY(EditAnywhere)
		UAnimSequence* ActionAnim;

	UPROPERTY(EditAnywhere)
		USoundBase* HitSound;
	UPROPERTY(EditAnywhere)
		USoundBase* HitWallSound;
	UPROPERTY(EditAnywhere)
		USoundBase* StabSound;

	// Left Click...
	virtual void Fire() override;

	// Right Click...
	virtual void Action() override;

	//// Put Weapon..
	//virtual void Draw(AFPSCharacter* player) override;


	bool CheckBackAttack(AFPSCharacter* Actor);

	void SetHit();
	void CheckAttack();
	virtual void ShuffleShotAnim() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnDecal(FHitResult Hit, EDecalPoolList Type);
};
