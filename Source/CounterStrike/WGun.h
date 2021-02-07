// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WBase.h"
#include "WGun.generated.h"

/**
 * 
 */
class UFPSCameraShake;
class USoundBase;
class UFPSCameraShake;
class UDecalComponent;
class AActorPool;
class ADecalActor;
class UNiagaraSystem;

struct FTimerHandle;

UCLASS()
class COUNTERSTRIKE_API AWGun : public AWBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* BulletTracer;

	UPROPERTY(EditAnywhere)
		uint8 GunDamage;
	UPROPERTY(EditAnywhere)
		float GunPenetration;

	bool RandomRecoilFlag = false;
	FTimerHandle ResetRecoilTimer;

	UFPSCameraShake* CamShake;


	uint8 ShotCount = 0;
	uint8 CurrentOneMagazineAmmoCount;
	uint8 ElseAmmoCount;

	UPROPERTY(EditAnywhere)
		uint8 OneMagazineAmmoCount;
	UPROPERTY(EditAnywhere)
		uint8 AmmoCount;

	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleParticle;


	UPROPERTY(EditAnywhere)
		FName MuzzleSocketName;
	UPROPERTY(EditAnywhere)
		FName ShellSocketName;
	UPROPERTY(EditAnywhere)
		FName BackSocketName;


	FVector Location;
	FRotator Rotation;
	FVector End;


	virtual void AmmoReload();
	virtual void SpawnShell() {};

public:
	uint8 GetShotCount() { return ShotCount; }
	uint8 GetOneMagazineAmmo() { return OneMagazineAmmoCount; }
	uint8 GetCurrentAmmoCount() { return CurrentOneMagazineAmmoCount; }
	uint8 GetElseAmmoCount() { return ElseAmmoCount; }
	FName GetBackSocketName() { return BackSocketName; }

	AWGun();

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim_2;

	UPROPERTY(EditAnywhere)
		UAnimSequence* ReloadAnim;

	UPROPERTY(EditAnywhere)
		USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
		USoundBase* DryMuzzleSound;

	//UPROPERTY(EditAnywhere)
	//	float MaxRange = 5000.f;

	// Left Click...
	virtual void GunShotMethod();
	virtual void DecreaseAmmo();
	virtual void ShakingCamera();
	virtual void PlayShotSound();
	virtual void Fire() override;
	virtual void Action() override;

	virtual void StopFire() override;

	virtual void Reload();
	virtual void RecoilEndVec();

	virtual void ResetTimeHandle() override;

	void RefreshAmmoCount();
	void SetCamShake(TSubclassOf<UCameraShake> CamComp);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void ShuffleShotAnim() override;

	void CharacterVelocityToEffectAim();
	void SpawnDecal(FHitResult Hit, EDecalPoolList Type);


	void CheckPenetrationShot(FHitResult Point, FVector Direction);
	void PenetrationShot(FHitResult Point, FVector Direction, float Distance);
};
