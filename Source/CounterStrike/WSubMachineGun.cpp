// Fill out your copyright notice in the Description page of Project Settings.


#include "WSubMachineGun.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"
#include "Kismet/GameplayStatics.h"

AWSubMachineGun::AWSubMachineGun()
{

	BackSocketName = "Mac10_Back";

	RunSpeedRatio = 0.91f;
}

void AWSubMachineGun::ChangeRecoilDirection()
{
	//if (ShotCount >)
	//{
	//	Direction = true;
	//}

	if (ShotCount > 15)
	{
		Direction = false;
	}
}

void AWSubMachineGun::SpawnShell()
{
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	AStaticMeshActor* Shell = nullptr;

	if (ActorPool)
	{
		Shell = ActorPool->GetMesh(EShellPoolList::ESP_RIFLE);
		if (Shell)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Shell Spawn!"));

			Shell->SetActorLocation(Player->GetCurrentFPSMesh()->GetSocketLocation(ShellSocketName));
			Shell->GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
			Shell->SetActorHiddenInGame(false);

			Shell->GetStaticMeshComponent()->AddImpulse(Player->FPSCameraComponent->GetRightVector() * 60.f);
		}
	}
}