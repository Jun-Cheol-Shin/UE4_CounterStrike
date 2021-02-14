// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterStatComponent.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

#include "Math/Vector.h"
#include "Particles/ParticleSystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "FPSCharacter.h"
#include "Global.h"
#include "FPSHUDWidget.h"

// Sets default values for this component's properties
UFPSCharacterStatComponent::UFPSCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	InitCharacterHP();
	PurchaseKavlarAndHelmet();

	bIsDead = false;
}

void UFPSCharacterStatComponent::InitDollar()
{
	CurrentDollar = 1000;
}

void UFPSCharacterStatComponent::SetDollar(uint32 Dollar)
{
	CurrentDollar = Dollar;
}


void UFPSCharacterStatComponent::InitCharacterHP()
{
	CurrentHP = MaxHP;
}

void UFPSCharacterStatComponent::PurchaseKevlar()
{
	CurrentKevlar = MaxKevlar * 0.5;
}

void UFPSCharacterStatComponent::PurchaseKavlarAndHelmet()
{
	CurrentKevlar = MaxKevlar;
}


// Called when the game starts
void UFPSCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UFPSCharacterStatComponent::SetArrayWeapon(AWBase* Weapon)
{
	WeaponArray.AddUnique(Weapon);
	UGameplayStatics::SpawnSoundAttached(GunPickupSound, Weapon->WeaponComponent);
}

AWBase* UFPSCharacterStatComponent::GetSelectWeapon(EWeaponNum Number)
{
	for (int i = 0; i < WeaponArray.Num(); ++i)
	{
		if (WeaponArray[i]->eWeaponNum == Number)
		{
			return WeaponArray[i];
		}
	}

	return nullptr;
}

void UFPSCharacterStatComponent::RemoveSelectWeapon(EWeaponNum Number)
{
	WeaponArray.Remove(GetSelectWeapon(Number));
}


EDamagedDirectionType UFPSCharacterStatComponent::CheckDirection(AFPSCharacter* DamagedActor, FVector Direction)
{
	float DeathValue = FVector::DotProduct(Direction, DamagedActor->FPSCameraComponent->GetForwardVector());

	if (DeathValue > 0.7f)
	{
		// back..
		return EDamagedDirectionType::EDDT_BACK;
	}

	else if (DeathValue < -0.7f)
	{
		//	front..
		return EDamagedDirectionType::EDDT_FRONT;
	}

	FVector CrossVac = FVector::CrossProduct(DamagedActor->FPSCameraComponent->GetForwardVector(), -Direction);

	if (CrossVac.Z > 0)
	{
		return EDamagedDirectionType::EDDT_RIGHT;
	}

	else
	{
		return EDamagedDirectionType::EDDT_LEFT;
	}

	return EDamagedDirectionType::EDDT_ALL;
}

void UFPSCharacterStatComponent::GetDamage(AActor* DamagedActor, int16 Damage, float Penetration, EBoneHit HitType, FVector HitPoint, AActor* Causer, FVector Direction)
{

	CurrentKevlar -= FMath::RoundToInt(Damage * 0.5f);

	if (CurrentKevlar < 0)
	{
		CurrentKevlar = 0;
	}

	if (CurrentKevlar > 0)
	{
		switch (HitType)
		{
		case EBoneHit::EB_HEAD:
			UE_LOG(LogTemp, Warning, TEXT("Hit Head!"));
			CurrentHP -= FMath::RoundToInt(Damage * Penetration * 4.f);
			UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration * 4.f));
			break;
		case EBoneHit::EB_LEG:
			UE_LOG(LogTemp, Warning, TEXT("Hit Leg!"));
			CurrentHP -= FMath::RoundToInt(Damage * Penetration * 0.75f);
			UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration * 0.75f));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Hit!"));
			CurrentHP -= FMath::RoundToInt(Damage * Penetration);
			UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration));
			break;
		}
	}

	else
	{
		switch (HitType)
		{
		case EBoneHit::EB_HEAD:
			UE_LOG(LogTemp, Warning, TEXT("Hit Head!"));
			CurrentHP -= FMath::RoundToInt(Damage * 4.f);
			break;
		case EBoneHit::EB_LEG:
			UE_LOG(LogTemp, Warning, TEXT("Hit Leg!"));
			CurrentHP -= FMath::RoundToInt(Damage * 0.75f);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Hit!"));
			CurrentHP -= Damage;
			break;
		}
	}


	if (CurrentHP < 0)
	{
		CurrentHP = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("HP : %d"), CurrentHP);

	AFPSCharacter* Damaged = Cast<AFPSCharacter>(DamagedActor);
	if (Damaged)
	{
		if (Damaged->GetFPSUIWidget())
		{
			Damaged->GetFPSUIWidget()->SetDamageUI(CheckDirection(Damaged, Direction));
		}
		Damaged->GetFPSCharacterMovement()->ResetSpeedRatio();
		if (CurrentHP <= 0)
		{
			if (!bIsDead)
			{
				Death(Damaged, Direction, HitType, Causer);
			}
		}

		switch (HitType)
		{
		case EBoneHit::EB_HEAD:
			UGameplayStatics::SpawnSoundAttached(HelmetHit, Damaged->GetMesh());
			break;
		default:
			UGameplayStatics::SpawnSoundAttached(KevlarHit, Damaged->GetMesh());
			break;
		}
	}

	if (HitPoint == FVector::ZeroVector)
		return;

	FTransform transform;
	transform.SetLocation(HitPoint);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, transform, true);


	//UE_LOG(LogTemp, Warning, TEXT("%d %d"), CurrentHP, CurrentKevlar);
}


void UFPSCharacterStatComponent::CheckDeath(AFPSCharacter* DeadActor, FVector Direction, EBoneHit HitType, AActor* Causer)
{

	EDamagedDirectionType DirecitonType = CheckDirection(DeadActor, Direction);

	//float DeathValue = FVector::DotProduct(Direction, DeadActor->FPSCameraComponent->GetForwardVector());

	UGameplayStatics::SpawnSoundAttached(DeathSound, DeadActor->GetMesh());

	//UE_LOG(LogTemp, Warning, TEXT("Deathvalue = %.1f"), DeathValue);

	if (HitType == EBoneHit::EB_HEAD)
	{
		DeadActor->SetActorRotation(FRotator(0, UKismetMathLibrary::FindLookAtRotation(DeadActor->GetActorLocation(), Causer->GetActorLocation()).Yaw, 0));

		DeathNum = EKindOfDeath::EKOD_HEAD;
		UE_LOG(LogTemp, Warning, TEXT("HEAD!!!"));
		UGameplayStatics::SpawnSoundAttached(HeadShotSound, DeadActor->GetMesh());
		return;
	}


	if (DirecitonType == EDamagedDirectionType::EDDT_BACK)
	{
		// need Crouch,, gut,,, Head,,,

		if (DeadActor->GetFPSCharacterMovement()->IsCrouchHeld)
		{
			DeathNum = EKindOfDeath::EKOD_CROUCH;
			UE_LOG(LogTemp, Warning, TEXT("Crouch!!!"));
			return;
		}
	
		else if (HitType == EBoneHit::EB_LEG)
		{
			DeathNum = EKindOfDeath::EKOD_GUTS;
			UE_LOG(LogTemp, Warning, TEXT("GUTS!!!"));
			return;
		}

		else
		{
			DeathNum = EKindOfDeath::EKOD_FRONT;
			UE_LOG(LogTemp, Warning, TEXT("FRONT!!!"));
			return;
		}
	}

	else if (DirecitonType == EDamagedDirectionType::EDDT_FRONT)
	{
		DeathNum = EKindOfDeath::EKOD_BACK;

		UE_LOG(LogTemp, Warning, TEXT("BACK!!!"));
		return;
	}

	//FVector CrossVac = FVector::CrossProduct(DeadActor->FPSCameraComponent->GetForwardVector(), -Direction);

	else if (DirecitonType == EDamagedDirectionType::EDDT_RIGHT)
	{
		DeathNum = EKindOfDeath::EKOD_LEFT;
		UE_LOG(LogTemp, Warning, TEXT("Left!!!"));
		return;
	}

	else if(DirecitonType == EDamagedDirectionType::EDDT_LEFT)
	{
		DeathNum = EKindOfDeath::EKOD_RIGHT;
		UE_LOG(LogTemp, Warning, TEXT("Right!!!"));
		return;
	}
}


void UFPSCharacterStatComponent::Death(AFPSCharacter* DeathActor, FVector Direction, EBoneHit HitType, AActor* Causer)
{
	bIsDead = true;
	DeathActor->ChangeViewCamera();
	DeathActor->GetCapsuleComponent()->SetCollisionProfileName("Dead");
	DeathActor->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CheckDeath(DeathActor, Direction, HitType, Causer);
}


void UFPSCharacterStatComponent::Revive(AFPSCharacter* ReviveActor)
{
	bIsDead = false;
	DeathNum = EKindOfDeath::EKOD_NONE;
	ReviveActor->GetCapsuleComponent()->SetCollisionProfileName("Alive");
	ReviveActor->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}