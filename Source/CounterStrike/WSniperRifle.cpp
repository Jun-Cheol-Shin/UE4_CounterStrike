// Fill out your copyright notice in the Description page of Project Settings.


#include "WSniperRifle.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ActorPool.h"

AWSniperRifle::AWSniperRifle()
{

	eWeaponNum = EWeaponNum::E_Rifle;
	eGunNumber = EGunNumber::RIFLE;
	EScopeState = EScope::NOSCOPE;

	bCanAutoFire = false;

	SocketName = TEXT("Rifle_Socket");

	RunSpeedRatio = 0.75f;
}

EScope AWSniperRifle::GetSniperScopeState()
{
	return EScopeState; 
}

void AWSniperRifle::SetSniperScopeState(EScope State)
{
	EScopeState = State;
}

void AWSniperRifle::RecoilEndVec()
{
	if (EScope::NOSCOPE == EScopeState)
	{
		Rotation.Yaw += FMath::RandRange(-NoScopeVerticalRecoilValue, NoScopeVerticalRecoilValue);
		Rotation.Pitch += FMath::RandRange(-NoScopeHorizontalRecoilValue, NoScopeHorizontalRecoilValue);
	}
}

void AWSniperRifle::ShakingCamera()
{
	if (CamShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(Player->GetFPSCameraAttack(), NoScopeVerticalRecoilValue + 15.f);
	}
}

void AWSniperRifle::Fire()
{
	if (Player)
	{	
		CancelScope();

		if (CurrentOneMagazineAmmoCount > 0 && EScopeState != EScope::NOSCOPE)
		{
			GetWorld()->GetTimerManager().SetTimer(SniperTimerHandle, [this]() {

				GetWorld()->GetTimerManager().ClearTimer(SniperTimerHandle);
				ScopeFunction();

			}, GetAttackDelay(), false);
		}
		Super::Fire();
	}
}

void AWSniperRifle::Action()
{
	Super::Action();

	switch (EScopeState)
	{
	case EScope::NOSCOPE:
		EScopeState = EScope::SCOPE;
		break;
	case EScope::SCOPE:
		EScopeState = EScope::DOUBLESCOPE;
		break;
	case EScope::DOUBLESCOPE:
		EScopeState = EScope::NOSCOPE;
		break;
	}


	ScopeFunction();
}

void AWSniperRifle::CancelScope()
{
	Player->ChangeViewPort(false);
	Player->FPSCameraComponent->FieldOfView = 90.f;

	//Player->StopWalk();
}

void AWSniperRifle::ScopeFunction()
{
	UGameplayStatics::SpawnSoundAttached(ActionSound, Player->GetMesh(), SocketName);

	switch (EScopeState)
	{
	case EScope::NOSCOPE:
		CancelScope();
		break;
	case EScope::SCOPE:
		//if (ScopeWalk)
		//{
		//	Player->StartWalk();
		//}
		Player->ChangeViewPort(true);
		Player->FPSCameraComponent->FieldOfView = 45.f;
		break;
	case EScope::DOUBLESCOPE:
		//if (ScopeWalk)
		//{
		//	Player->StartWalk();
		//}
		Player->ChangeViewPort(true);
		Player->FPSCameraComponent->FieldOfView = 22.5f;
		break;
	}
}

void AWSniperRifle::ShuffleShotAnim()
{
	uint8 random = FMath::RandRange(0, 3);

	switch (random)
	{
	case 1:
		CurrentPlayingAnim = AttackAnim_2;
		break;
	case 2:
		CurrentPlayingAnim = AttackAnim_3;
		break;
	default:
		CurrentPlayingAnim = AttackAnim;
		break;
	}

	ShotCount += 60;
	Super::ShuffleShotAnim();
}

void AWSniperRifle::SpawnShell()
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
		Shell = ActorPool->GetMesh(EShellPoolList::ESP_BIGRIFLE);
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