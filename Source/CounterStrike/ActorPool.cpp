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

void AActorPool::BeginPlay()
{
	Super::BeginPlay();

	Initialized();
}

void AActorPool::Initialized()
{
	DecalPool.Add(TEXT("BulletHole"), new FPoolData<ADecalActor*>());
	DecalPool.Add(TEXT("KnifeDecal"), new FPoolData<ADecalActor*>());
	DecalPool.Add(TEXT("Explosion"), new FPoolData<ADecalActor*>());
	DecalPool.Add(TEXT("Blood"), new FPoolData<ADecalActor*>());

	CreateDecal(TEXT("Blueprint'/Game/BluePrint/Decal/BulletHole_1.BulletHole_1_C'"), TEXT("BulletHole"));
	CreateDecal(TEXT("Blueprint'/Game/BluePrint/Decal/KnifeDecal.KnifeDecal_C'"), TEXT("KnifeDecal"));
	CreateDecal(TEXT("Blueprint'/Game/BluePrint/Decal/Explosion.Explosion_C'"), TEXT("Explosion"));
	CreateDecal(TEXT("Blueprint'/Game/BluePrint/Decal/BloodSplatter.BloodSplatter_C'"), TEXT("Blood"));


	ShellPool.Add(TEXT("Pistol"), new FPoolData<AStaticMeshActor*>());
	ShellPool.Add(TEXT("762"), new FPoolData<AStaticMeshActor*>());
	ShellPool.Add(TEXT("BigRifle"), new FPoolData<AStaticMeshActor*>());
	ShellPool.Add(TEXT("Rifle"), new FPoolData<AStaticMeshActor*>());
	ShellPool.Add(TEXT("Shotgun"), new FPoolData<AStaticMeshActor*>());

	CreateStaticMesh(TEXT("Blueprint'/Game/BluePrint/Shell/PistolShell.PistolShell_C'"), TEXT("Pistol"));
	CreateStaticMesh(TEXT("Blueprint'/Game/BluePrint/Shell/762Shell.762Shell_C'"), TEXT("762"));
	CreateStaticMesh(TEXT("Blueprint'/Game/BluePrint/Shell/BigRifleShell.BigRifleShell_C'"), TEXT("BigRifle"));
	CreateStaticMesh(TEXT("Blueprint'/Game/BluePrint/Shell/RifleShell.RifleShell_C'"), TEXT("Rifle"));
	CreateStaticMesh(TEXT("Blueprint'/Game/BluePrint/Shell/ShotgunShell.ShotgunShell_C'"), TEXT("Shotgun"));
}


void AActorPool::CreateStaticMesh(FName Path, FString ArrayName)
{
	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));
	FTransform transform = FTransform::Identity;

	for (int i = 0; i < ShellCount; ++i)
	{
		AStaticMeshActor* ShellActor = GetWorld()->SpawnActor<AStaticMeshActor>(GeneratedBP, transform);
		ShellActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
		ShellActor->SetActorHiddenInGame(true);
		ShellActor->SetAutoDestroyWhenFinished(false);
		ShellActor->SetLifeSpan(0.f);
		ShellPool[ArrayName]->ArrayList.Add(ShellActor);
	}
}

AStaticMeshActor* AActorPool::GetMesh(EShellPoolList Type)
{
	AStaticMeshActor* retval = nullptr;
	FTimerHandle Timer;

	switch (Type)
	{
	case EShellPoolList::ESP_PISTOL:
		if (ShellPool["Pistol"]->ArrayList.Num() > 0)
		{
			retval = ShellPool["Pistol"]->ArrayList.Pop();
			retval->GetStaticMeshComponent()->SetSimulatePhysics(true);
			ShellPool["Pistol"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!ShellPool["Pistol"]->UseList.IsEmpty())
				{
					AStaticMeshActor* actor;
					ShellPool["Pistol"]->UseList.Dequeue(actor);
					ReturnMesh(actor, EShellPoolList::ESP_PISTOL);
				}

			}, ShellLifeTimeSpan, false);
		}
		break;

	case EShellPoolList::ESP_762:
		if (ShellPool["762"]->ArrayList.Num() > 0)
		{
			retval = ShellPool["762"]->ArrayList.Pop();
			retval->GetStaticMeshComponent()->SetSimulatePhysics(true);
			ShellPool["762"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!ShellPool["762"]->UseList.IsEmpty())
				{
					AStaticMeshActor* actor;
					ShellPool["762"]->UseList.Dequeue(actor);
					ReturnMesh(actor, EShellPoolList::ESP_762);
				}

			}, ShellLifeTimeSpan, false);
		}
		break;
	case EShellPoolList::ESP_BIGRIFLE:
		if (ShellPool["BigRifle"]->ArrayList.Num() > 0)
		{
			retval = ShellPool["BigRifle"]->ArrayList.Pop();
			retval->GetStaticMeshComponent()->SetSimulatePhysics(true);
			ShellPool["BigRifle"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!ShellPool["BigRifle"]->UseList.IsEmpty())
				{
					AStaticMeshActor* actor;
					ShellPool["BigRifle"]->UseList.Dequeue(actor);
					ReturnMesh(actor, EShellPoolList::ESP_BIGRIFLE);
				}

			}, ShellLifeTimeSpan, false);
		}
		break;

	case EShellPoolList::ESP_RIFLE:
		if (ShellPool["Rifle"]->ArrayList.Num() > 0)
		{
			retval = ShellPool["Rifle"]->ArrayList.Pop();
			retval->GetStaticMeshComponent()->SetSimulatePhysics(true);
			ShellPool["Rifle"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!ShellPool["Rifle"]->UseList.IsEmpty())
				{
					AStaticMeshActor* actor;
					ShellPool["Rifle"]->UseList.Dequeue(actor);
					ReturnMesh(actor, EShellPoolList::ESP_RIFLE);
				}

			}, ShellLifeTimeSpan, false);
		}
		break;

	case EShellPoolList::ESP_SHOTGUN:
		if (ShellPool["Shotgun"]->ArrayList.Num() > 0)
		{
			retval = ShellPool["Shotgun"]->ArrayList.Pop();
			retval->GetStaticMeshComponent()->SetSimulatePhysics(true);
			ShellPool["Shotgun"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!ShellPool["Shotgun"]->UseList.IsEmpty())
				{
					AStaticMeshActor* actor;
					ShellPool["Shotgun"]->UseList.Dequeue(actor);
					ReturnMesh(actor, EShellPoolList::ESP_SHOTGUN);
				}

			}, ShellLifeTimeSpan, false);
		}
		break;

	default:
		break;
	}

	return retval;
}

void AActorPool::ReturnMesh(AStaticMeshActor* Actor, EShellPoolList Type)
{
	UGameplayStatics::SpawnSoundAttached(ShellSound, Actor->GetStaticMeshComponent());

	if (Actor)
	{
		switch (Type)
		{
		case EShellPoolList::ESP_PISTOL:
			Actor->GetStaticMeshComponent()->SetSimulatePhysics(false);
			Actor->SetActorHiddenInGame(true);
			ShellPool["Pistol"]->ArrayList.Add(Actor);
			break;

		case EShellPoolList::ESP_762:
			Actor->GetStaticMeshComponent()->SetSimulatePhysics(false);
			Actor->SetActorHiddenInGame(true);
			ShellPool["762"]->ArrayList.Add(Actor);
			break;
		case EShellPoolList::ESP_BIGRIFLE:
			Actor->GetStaticMeshComponent()->SetSimulatePhysics(false);
			Actor->SetActorHiddenInGame(true);
			ShellPool["BigRifle"]->ArrayList.Add(Actor);
			break;
		case EShellPoolList::ESP_RIFLE:
			Actor->GetStaticMeshComponent()->SetSimulatePhysics(false);
			Actor->SetActorHiddenInGame(true);
			ShellPool["Rifle"]->ArrayList.Add(Actor);
			break;
		case EShellPoolList::ESP_SHOTGUN:
			Actor->GetStaticMeshComponent()->SetSimulatePhysics(false);
			Actor->SetActorHiddenInGame(true);
			ShellPool["Shotgun"]->ArrayList.Add(Actor);
			break;
		}
	}
}

void AActorPool::CreateDecal(FName Path, FString ArrayName)
{
	//FName Path = "Blueprint'/Game/BluePrint/Decal/BulletHole_1.BulletHole_1_C'";

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));
	FTransform transform = FTransform::Identity;


	for (int i = 0; i < DecalCount; ++i)
	{
		ADecalActor* decalActor = GetWorld()->SpawnActor<ADecalActor>(GeneratedBP, transform);
		if (decalActor)
		{
			decalActor->SetActorHiddenInGame(true);
			decalActor->SetAutoDestroyWhenFinished(false);
			decalActor->SetLifeSpan(0.f);
			DecalPool[ArrayName]->ArrayList.Add(decalActor);
		}	
	}
}

void AActorPool::ReturnDecal(ADecalActor* Decal, EDecalPoolList Type)
{
	if (Decal)
	{
		switch (Type)
		{
		case EDecalPoolList::EDP_BLOOD:
			Decal->SetActorHiddenInGame(true);
			DecalPool["Blood"]->ArrayList.Add(Decal);
			break;
		case EDecalPoolList::EDP_BULLETHOLE:
			//UE_LOG(LogTemp, Warning, TEXT("Return Decal"));
			Decal->SetActorHiddenInGame(true);
			DecalPool["BulletHole"]->ArrayList.Add(Decal);
			break;

		case EDecalPoolList::EDP_SCAR:
			//UE_LOG(LogTemp, Warning, TEXT("Return Decal"));
			Decal->SetActorHiddenInGame(true);
			DecalPool["KnifeDecal"]->ArrayList.Add(Decal);
			break;

		case EDecalPoolList::EDP_EXPLOSION:
			//UE_LOG(LogTemp, Warning, TEXT("Return Decal"));
			Decal->SetActorHiddenInGame(true);
			DecalPool["Explosion"]->ArrayList.Add(Decal);
			break;
		}
	}
}

ADecalActor* AActorPool::GetDecal(EDecalPoolList Type)
{
	ADecalActor* retval = nullptr;

	FTimerHandle Timer;

	switch (Type)
	{
	case EDecalPoolList::EDP_BLOOD:
		if (DecalPool["Blood"]->ArrayList.Num() > 0)
		{
			retval = DecalPool["Blood"]->ArrayList.Pop();
			retval->GetDecal()->SetFadeScreenSize(0.f);
			retval->GetDecal()->DecalSize = FVector(30.0f, 30.0f, 30.0f);
			DecalPool["Blood"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!DecalPool["Blood"]->UseList.IsEmpty())
				{
					ADecalActor* actor;
					DecalPool["Blood"]->UseList.Dequeue(actor);

					ReturnDecal(actor, EDecalPoolList::EDP_BLOOD);
				}

			}, DecalLifeTimeSpan, false);
		}
		break;

	case EDecalPoolList::EDP_BULLETHOLE:
		if (DecalPool["BulletHole"]->ArrayList.Num() > 0)
		{
			retval = DecalPool["BulletHole"]->ArrayList.Pop();
			retval->GetDecal()->SetFadeScreenSize(0.f);
			retval->GetDecal()->DecalSize = FVector(5.0f, 5.0f, 5.0f);
			DecalPool["BulletHole"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!DecalPool["BulletHole"]->UseList.IsEmpty())
				{
					ADecalActor* actor;
					DecalPool["BulletHole"]->UseList.Dequeue(actor);

					ReturnDecal(actor, EDecalPoolList::EDP_BULLETHOLE);
				}

			}, DecalLifeTimeSpan, false);
		}
		break;

	case EDecalPoolList::EDP_SCAR:
		if (DecalPool["KnifeDecal"]->ArrayList.Num() > 0)
		{
			retval = DecalPool["KnifeDecal"]->ArrayList.Pop();
			retval->GetDecal()->SetFadeScreenSize(0.f);
			retval->GetDecal()->DecalSize = FVector(5.0f, 5.0f, 5.0f);
			DecalPool["KnifeDecal"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!DecalPool["KnifeDecal"]->UseList.IsEmpty())
				{
					ADecalActor* actor;
					DecalPool["KnifeDecal"]->UseList.Dequeue(actor);

					ReturnDecal(actor, EDecalPoolList::EDP_SCAR);
				}

			}, DecalLifeTimeSpan, false);
		}
		break;

	case EDecalPoolList::EDP_EXPLOSION:
		if (DecalPool["Explosion"]->ArrayList.Num() > 0)
		{
			retval = DecalPool["Explosion"]->ArrayList.Pop();
			retval->GetDecal()->SetFadeScreenSize(0.f);
			retval->GetDecal()->DecalSize = FVector(5.0f, 5.0f, 5.0f);
			DecalPool["Explosion"]->UseList.Enqueue(retval);

			GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {

				if (!DecalPool["Explosion"]->UseList.IsEmpty())
				{
					ADecalActor* actor;
					DecalPool["Explosion"]->UseList.Dequeue(actor);

					ReturnDecal(actor, EDecalPoolList::EDP_SCAR);
				}

			}, DecalLifeTimeSpan, false);
		}
		break;

	default:
		break;
	}

	return retval;
}
