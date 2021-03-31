// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorPool.h"
#include "GameFramework/Actor.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Global.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "FPSCharacter.h"
#include "GameFramework/Actor.h"

#include "WBase.h"
#include "Net/UnrealNetwork.h"


AActorPool::AActorPool()
{
	static ConstructorHelpers::FClassFinder<ADecalActor> Bullet(TEXT("Blueprint'/Game/BluePrint/Decal/BulletHole_1.BulletHole_1_C'"));
	if (Bullet.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Bullet Success!"));
		BulletDecalBluePrint = Bullet.Class;
	}

	static ConstructorHelpers::FClassFinder<ADecalActor> Knife(TEXT("Blueprint'/Game/BluePrint/Decal/KnifeDecal.KnifeDecal_C'"));
	if (Knife.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Knife Success!"));
		KnifeDecalBluePrint = Knife.Class;
	}

	static ConstructorHelpers::FClassFinder<ADecalActor> Explosion(TEXT("Blueprint'/Game/BluePrint/Decal/Explosion.Explosion_C'"));
	if (Explosion.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Explosion Success!"));
		ExplosionDecalBluePrint = Explosion.Class;
	}

	static ConstructorHelpers::FClassFinder<ADecalActor> Blood(TEXT("Blueprint'/Game/BluePrint/Decal/BloodSplatter.BloodSplatter_C'"));
	if (Blood.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Blood Success!"));
		BloodDecalBluePrint = Blood.Class;
	}



	static ConstructorHelpers::FClassFinder<AStaticMeshActor> PistolShell(TEXT("Blueprint'/Game/BluePrint/Shell/PistolShell.PistolShell_C'"));
	if (PistolShell.Succeeded())
	{
		PistolBluePrint = PistolShell.Class;
	}

	static ConstructorHelpers::FClassFinder<AStaticMeshActor> Shell762(TEXT("Blueprint'/Game/BluePrint/Shell/762Shell.762Shell_C'"));
	if (Shell762.Succeeded())
	{
		Shell762BluePrint = Shell762.Class;
	}

	static ConstructorHelpers::FClassFinder<AStaticMeshActor> BigRifle(TEXT("Blueprint'/Game/BluePrint/Shell/BigRifleShell.BigRifleShell_C'"));
	if (BigRifle.Succeeded())
	{
		BigRifleBluePrint = BigRifle.Class;
	}

	static ConstructorHelpers::FClassFinder<AStaticMeshActor> Rifle(TEXT("Blueprint'/Game/BluePrint/Shell/RifleShell.RifleShell_C'"));
	if (Rifle.Succeeded())
	{
		RifleBluePrint = Rifle.Class;
	}

	static ConstructorHelpers::FClassFinder<AStaticMeshActor> ShotGun(TEXT("Blueprint'/Game/BluePrint/Shell/ShotgunShell.ShotgunShell_C'"));
	if (ShotGun.Succeeded())
	{
		ShotGunBluePrint = ShotGun.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> AK_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_AK47.BP_AK47_C'"));
	if (AK_C.Succeeded())
	{
		BP_Ak47 = AK_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> Knife_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Knife.BP_Knife_C'"));
	if (Knife_C.Succeeded())
	{
		BP_Knife = Knife_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> M4A1_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_M4A1.BP_M4A1_C'"));
	if (M4A1_C.Succeeded())
	{
		BP_M4a1 = M4A1_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> AWP_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Awp.BP_Awp_C'"));
	if (AWP_C.Succeeded())
	{
		BP_AWP = AWP_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> DEAGLE_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Deagle.BP_Deagle_C'"));
	if (DEAGLE_C.Succeeded())
	{
		BP_Deagle = DEAGLE_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> GLOCK_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Glock18.BP_Glock18_C'"));
	if (GLOCK_C.Succeeded())
	{
		BP_Glock18 = GLOCK_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> GRENADE_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Grenade.BP_Grenade_C'"));
	if (GRENADE_C.Succeeded())
	{
		BP_Grenade = GRENADE_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> SMOKE_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Smoke.BP_Smoke_C'"));
	if (SMOKE_C.Succeeded())
	{
		BP_Smoke = SMOKE_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> FLASH_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Flash.BP_Flash_C'"));
	if (FLASH_C.Succeeded())
	{
		BP_Flash = FLASH_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> MAC_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Mac10.BP_Mac10_C'"));
	if (MAC_C.Succeeded())
	{
		BP_Mac10 = MAC_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> NOVA_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Nova.BP_Nova_C'"));
	if (NOVA_C.Succeeded())
	{
		BP_Nova = NOVA_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> SCOUT_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Scout.BP_Scout_C'"));
	if (SCOUT_C.Succeeded())
	{
		BP_Scout = SCOUT_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> USP_C(TEXT("Blueprint'/Game/BluePrint/Weapon/BP_Usp.BP_Usp_C'"));
	if (USP_C.Succeeded())
	{
		BP_Usp = USP_C.Class;
	}


}


void AActorPool::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AActorPool::BeginPlay()
{
	Super::BeginPlay();
}


TSubclassOf<class ADecalActor> AActorPool::GetBulletHoleDecal()
{
	return BulletDecalBluePrint;
}

TSubclassOf<class ADecalActor> AActorPool::GetBloodDecal()
{
	return BloodDecalBluePrint;
}

TSubclassOf<class ADecalActor> AActorPool::GetKnifeDecal()
{
	return KnifeDecalBluePrint;
}

TSubclassOf<class ADecalActor> AActorPool::GetExplosionDecal()
{
	return ExplosionDecalBluePrint;
}

TSubclassOf<class AStaticMeshActor> AActorPool::Get762Shell()
{
	return Shell762BluePrint;
}

TSubclassOf<class AStaticMeshActor> AActorPool::GetBigRifleShell()
{
	return BigRifleBluePrint;
}

TSubclassOf<class AStaticMeshActor> AActorPool::GetRifleShell()
{
	return RifleBluePrint;
}

TSubclassOf<class AStaticMeshActor> AActorPool::GetPistolShell()
{
	return PistolBluePrint;
}

TSubclassOf<class AStaticMeshActor> AActorPool::GetShotgunShell()
{
	return ShotGunBluePrint;
}