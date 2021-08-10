// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Global.h"
#include "ActorPool.generated.h"



class AWBase;
class ADecalActor;
class AStaticMeshActor;
class AFPSCharacter;


UCLASS()
class COUNTERSTRIKE_API AActorPool : public AActor
{
	GENERATED_BODY()

private:
	TMap<FString, TArray<AWBase*>> WeaponPool;
	
	UPROPERTY(EditAnywhere)
		USoundBase* GunPickupSound;

	UPROPERTY(EditAnywhere)
		USoundBase* DeathSound;

	UPROPERTY(EditAnywhere)
		USoundBase* HeadShotSound;

	UPROPERTY(EditAnywhere)
		USoundBase* KevlarHit;

	UPROPERTY(EditAnywhere)
		USoundBase* HelmetHit;

	UPROPERTY(EditAnywhere)
		UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere)
		UParticleSystem* WallImpactParticle;


	TSubclassOf<class ADecalActor> BulletDecalBluePrint;
	TSubclassOf<class ADecalActor> BloodDecalBluePrint;
	TSubclassOf<class ADecalActor> KnifeDecalBluePrint;
	TSubclassOf<class ADecalActor> ExplosionDecalBluePrint;

	TSubclassOf<class AStaticMeshActor> Shell762BluePrint;
	TSubclassOf<class AStaticMeshActor> BigRifleBluePrint;
	TSubclassOf<class AStaticMeshActor> RifleBluePrint;
	TSubclassOf<class AStaticMeshActor> PistolBluePrint;
	TSubclassOf<class AStaticMeshActor> ShotGunBluePrint;

	TSubclassOf<class AWBase> BP_Ak47;
	TSubclassOf<class AWBase> BP_M4a1;
	TSubclassOf<class AWBase> BP_AWP;
	TSubclassOf<class AWBase> BP_Deagle;
	TSubclassOf<class AWBase> BP_Flash;
	TSubclassOf<class AWBase> BP_Glock18;
	TSubclassOf<class AWBase> BP_Grenade;
	TSubclassOf<class AWBase> BP_Mac10;
	TSubclassOf<class AWBase> BP_Nova;
	TSubclassOf<class AWBase> BP_Scout;
	TSubclassOf<class AWBase> BP_Smoke;
	TSubclassOf<class AWBase> BP_Usp;
	TSubclassOf<class AWBase> BP_Knife;


public:
	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const;
	virtual void BeginPlay() override;

	AActorPool();

	TSubclassOf<class AWBase> GetAK47() { return BP_Ak47; }
	TSubclassOf<class AWBase> GetAWP() { return BP_AWP; }
	TSubclassOf<class AWBase> GetM4A1() { return BP_M4a1; }
	TSubclassOf<class AWBase> GetDeagle() { return BP_Deagle; }
	TSubclassOf<class AWBase> GetFlash() { return BP_Flash; }
	TSubclassOf<class AWBase> GetGlock18() { return BP_Glock18; }
	TSubclassOf<class AWBase> GetGrenade() { return BP_Grenade; }
	TSubclassOf<class AWBase> GetMac10() { return BP_Mac10; }
	TSubclassOf<class AWBase> GetNova() { return BP_Nova; }
	TSubclassOf<class AWBase> GetScout() { return BP_Scout; }
	TSubclassOf<class AWBase> GetSmoke() { return BP_Smoke; }
	TSubclassOf<class AWBase> GetUSP() { return BP_Usp; }
	TSubclassOf<class AWBase> GetKnife() { return BP_Knife; }

	USoundBase* GetPickupSound()		{ return GunPickupSound; }
	USoundBase* GetDeathSound()			{ return DeathSound; }
	USoundBase* GetKevlarShotSound()	{ return KevlarHit; }
	USoundBase* GetHelmetShotSound()	{ return HelmetHit; }
	USoundBase* GetHeadShotSound()		{ return HeadShotSound; }

	UParticleSystem* GetBlood()			{ return BloodParticle; }
	UParticleSystem* GetImpact()		{ return WallImpactParticle; }

	TSubclassOf<class ADecalActor> GetBulletHoleDecal();
	TSubclassOf<class ADecalActor> GetBloodDecal();
	TSubclassOf<class ADecalActor> GetKnifeDecal();
	TSubclassOf<class ADecalActor> GetExplosionDecal();

	TSubclassOf<class AStaticMeshActor> Get762Shell();
	TSubclassOf<class AStaticMeshActor> GetBigRifleShell();
	TSubclassOf<class AStaticMeshActor> GetRifleShell();
	TSubclassOf<class AStaticMeshActor> GetPistolShell();
	TSubclassOf<class AStaticMeshActor> GetShotgunShell();
};
