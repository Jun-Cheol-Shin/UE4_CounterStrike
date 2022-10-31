// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WBase.h"
#include "WGun.generated.h"

/**
 * 
 */
class UMyMatineeCameraShake;
class USoundBase;
class UDecalComponent;
class AActorPool;
class ADecalActor;
class UNiagaraSystem;
class AStaticMeshActor;

struct FTimerHandle;

UCLASS()
class COUNTERSTRIKE_API AWGun : public AWBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		bool InfinityAmmo = false;

	UPROPERTY(EditAnywhere)
		float PenatrateDecreaseDistanceRatio = 1.f;


	AStaticMeshActor* SpawnedShell;
	TSubclassOf<class AStaticMeshActor> BulletDecalBluePrint;

	UPROPERTY(EditAnywhere)
		UNiagaraSystem* BulletTracer;

	UPROPERTY(EditAnywhere)
		uint8 GunDamage;
	UPROPERTY(EditAnywhere)
		float GunPenetration;

	bool RandomRecoilFlag = false;
	FTimerHandle ResetRecoilTimer;

	UMyMatineeCameraShake* CamShake;


	uint8 ShotCount = 0;
	uint8 CurrentOneMagazineAmmoCount;
	uint8 ElseAmmoCount;

	UPROPERTY(EditAnywhere)
		uint8 OneMagazineAmmoCount;
	UPROPERTY(EditAnywhere)
		uint8 AmmoCount;

	uint8 CurrentAmmoCount = AmmoCount;

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
	virtual void SpawnShell();

public:
	virtual ~AWGun()
	{

	}
	AWGun();

	void InitAmmoCount();
	// �߿�..
	uint8 GetCurrentAmmoCount() { return CurrentOneMagazineAmmoCount; }
	uint8 GetElseAmmoCount() { return ElseAmmoCount; }

	void SetAmmoCount(uint8 CurrentAmmo, uint8 ElseAmmo);


	uint8 GetShotCount() { return ShotCount; }
	uint8 GetOneMagazineAmmo() { return OneMagazineAmmoCount; }
	FName GetBackSocketName() { return BackSocketName; }

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
	void SetCamShake(TSubclassOf<UCameraShakeBase> CamComp);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void ShuffleShotAnim() override;

	void CharacterVelocityToEffectAim();
	void SpawnDecal(FHitResult Hit, EDecalPoolList Type);


	FHitResult CheckPenetrationShot(const TArray<FHitResult>& Point, const FVector& Direction);
	FHitResult PenetrationShot(const FHitResult& Point, const FVector& Direction, float& Distance);


	void SpawnNiagra(FVector ParticleStart, FVector ParitcleEnd);
};
