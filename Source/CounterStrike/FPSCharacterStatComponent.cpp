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
#include "Net/UnrealNetwork.h"
#include "ActorPool.h"
#include "GameFramework/PlayerStart.h"
#include "Math/UnrealMathUtility.h"
#include "WGun.h"

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

AActorPool* UFPSCharacterStatComponent::GetActorPool()
{
	if (!ActorPool)
	{
		ActorPool = Cast<AActorPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorPool::StaticClass()));
	}

	return ActorPool;
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
	UGameplayStatics::SpawnSoundAttached(GetActorPool()->GetPickupSound(), Weapon->WeaponComponent);
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
	// 상대의 앞벡터와 총알의 방향 벡터를 내적 계산
	float DeathValue = FVector::DotProduct(Direction, DamagedActor->GetActorForwardVector());

	// 0.7보다 작거나, -0.7보다 작으면 왼쪽, 오른쪽으로 UI를 표시해야 하기 때문에 if문을 작성한다.
	
	// 양수일 경우 서로 평행이 되는 방향
	if (DeathValue > 0.7f)
	{
		// back..
		return EDamagedDirectionType::EDDT_BACK;
	}

	// 음수는 서로 교차가 되는 방향
	else if (DeathValue < -0.7f)
	{
		//	front..
		return EDamagedDirectionType::EDDT_FRONT;
	}

	// 그 외의 경우는 측면으로 결과값을 리턴한다.

	// 측면은 외적으로 계산
	FVector CrossVac = FVector::CrossProduct(DamagedActor->GetActorForwardVector(), -Direction);

	// 외적의 오른손 법칙에 따라 양수면..
	if (CrossVac.Z > 0)
	{
		return EDamagedDirectionType::EDDT_RIGHT;
	}

	// 음수면 왼쪽..
	else
	{
		return EDamagedDirectionType::EDDT_LEFT;
	}

	return EDamagedDirectionType::EDDT_ALL;
}

void UFPSCharacterStatComponent::GetDamage(int16 Damage, float Penetration, AActor* Causer, EBoneHit HitType, FVector Direction)
{
	if (bIsDead) return;
		
	if (!bGodMode)
	{

		if (CurrentKevlar < 0) CurrentKevlar = 0;

		if (CurrentKevlar > 0)
		{
			CurrentKevlar -= FMath::RoundToInt(Damage * 0.5f);

			switch (HitType)
			{
			case EBoneHit::EB_HEAD:
				CurrentHP -= FMath::RoundToInt(Damage * Penetration * 4.f);
				//UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration * 4.f));
				break;
			case EBoneHit::EB_LEG:
				CurrentHP -= FMath::RoundToInt(Damage * Penetration * 0.75f);
				//UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration * 0.75f));
				break;
			default:
				CurrentHP -= FMath::RoundToInt(Damage * Penetration);
				//UE_LOG(LogTemp, Warning, TEXT("%d"), FMath::RoundToInt(Damage * Penetration));
				break;
			}
		}

		else
		{
			switch (HitType)
			{
			case EBoneHit::EB_HEAD:
				//UE_LOG(LogTemp, Warning, TEXT("Hit Head!"));
				CurrentHP -= FMath::RoundToInt(Damage * 4.f);
				break;
			case EBoneHit::EB_LEG:
				//UE_LOG(LogTemp, Warning, TEXT("Hit Leg!"));
				CurrentHP -= FMath::RoundToInt(Damage * 0.75f);
				break;
			default:
				//UE_LOG(LogTemp, Warning, TEXT("Hit!"));
				CurrentHP -= Damage;
				break;
			}
		}

		if (CurrentHP < 0)
		{
			CurrentHP = 0;
		}
		//UE_LOG(LogTemp, Warning, TEXT("HP : %d"), CurrentHP);
	}

	AFPSCharacter* Damaged = Cast<AFPSCharacter>(GetOwner());
	if (Damaged)
	{
		if (CurrentHP <= 0)
		{
			Death(Damaged, Direction, HitType, Causer);
			Revive(Damaged, ReviveTime);

			AFPSCharacter* CauserFPS = Cast<AFPSCharacter>(Causer);
			if (CauserFPS)
			{
				if (CauserFPS->GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
				{
					CauserFPS->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->InitAmmoCount();
				}
				CauserFPS->GetFPSUIWidget()->SetAmmoCount(CauserFPS);
			}

			////Damaged->DoSomethingOnServer(CauserFPS->GetFPSCharacterStatComponent()->GetKillCount(), CauserFPS);
			////Damaged->KillEvent(CauserFPS->GetFPSCharacterStatComponent()->GetKillCount(), CauserFPS);
			//}
		}

		switch (HitType)
		{
		case EBoneHit::EB_HEAD:
			UGameplayStatics::SpawnSoundAttached(GetActorPool()->GetHelmetShotSound(), Damaged->GetMesh());
			break;
		default:
			UGameplayStatics::SpawnSoundAttached(GetActorPool()->GetKevlarShotSound(), Damaged->GetMesh());
			break;
		}

		/*if (Causer->GetLocalRole() < ROLE_Authority)
		{
			if (AFPSCharacter* CauserCharacter = Cast<AFPSCharacter>(Causer))
			{
				if (CauserCharacter->IsLocallyControlled())
				{
					CauserCharacter->SyncClientSendDamaged(Damaged, CheckDirection(Damaged, Direction), CurrentHP, CurrentKevlar, HitType, Causer, Direction, ReviveTime);
				}
			}
		}*/

		if (AFPSCharacter* CauserCharacter = Cast<AFPSCharacter>(Causer))
		{
			CauserCharacter->SyncClientSendDamaged(Damaged, CheckDirection(Damaged, Direction), CurrentHP, CurrentKevlar, HitType, Causer, Direction, ReviveTime);
		}

	}

}


void UFPSCharacterStatComponent::CheckDeath(AFPSCharacter* DeadActor, FVector Direction, EBoneHit HitType, AActor* Causer)
{

	EDamagedDirectionType DirecitonType = CheckDirection(DeadActor, Direction);

	//float DeathValue = FVector::DotProduct(Direction, DeadActor->FPSCameraComponent->GetForwardVector());

	UGameplayStatics::SpawnSoundAttached(GetActorPool()->GetDeathSound(), DeadActor->GetMesh());

	//UE_LOG(LogTemp, Warning, TEXT("Deathvalue = %.1f"), DeathValue);

	if (HitType == EBoneHit::EB_HEAD)
	{
		DeadActor->SetActorRotation(FRotator(0, UKismetMathLibrary::FindLookAtRotation(DeadActor->GetActorLocation(), Causer->GetActorLocation()).Yaw, 0));

		DeathNum = EKindOfDeath::EKOD_HEAD;
		//UE_LOG(LogTemp, Warning, TEXT("HEAD!!!"));
		UGameplayStatics::SpawnSoundAttached(GetActorPool()->GetHeadShotSound(), DeadActor->GetMesh());
		return;
	}


	switch (DirecitonType)
	{
	case EDamagedDirectionType::EDDT_BACK:
		if (DeadActor->IsCrouchHeld)
		{
			DeathNum = EKindOfDeath::EKOD_CROUCH;
			//UE_LOG(LogTemp, Warning, TEXT("Crouch!!!"));
			return;
		}

		else if (HitType == EBoneHit::EB_GUTS)
		{
			DeathNum = EKindOfDeath::EKOD_GUTS;
			//UE_LOG(LogTemp, Warning, TEXT("GUTS!!!"));
			return;
		}

		else
		{
			DeathNum = EKindOfDeath::EKOD_FRONT;
			//UE_LOG(LogTemp, Warning, TEXT("FRONT!!!"));
			return;
		}
		break;
	case EDamagedDirectionType::EDDT_FRONT:
		DeathNum = EKindOfDeath::EKOD_BACK;
		break;
	case EDamagedDirectionType::EDDT_LEFT:
		DeathNum = EKindOfDeath::EKOD_RIGHT;
		break;
	case EDamagedDirectionType::EDDT_RIGHT:
		DeathNum = EKindOfDeath::EKOD_LEFT;
		break;
	}

}


void UFPSCharacterStatComponent::Death(AFPSCharacter* DeathActor, FVector Direction, EBoneHit HitType, AActor* Causer)
{
	bIsDead = true;
	DeathActor->ChangeViewCamera(false);
	DeathActor->GetCapsuleComponent()->SetCollisionProfileName("Dead");
	DeathActor->GetMesh()->SetCollisionProfileName("Ragdoll");
	CheckDeath(DeathActor, Direction, HitType, Causer);
}


void UFPSCharacterStatComponent::Revive(AFPSCharacter* ReviveActor, float ReviveSeconds)
{
	FTimerHandle ReviveTimer;

	TArray<AActor*> ActorArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorArray);

	if (ActorArray.Num() > 0)
	{
		int32 number = FMath::RandRange(0, ActorArray.Num() - 1);

		RevivePosition = ActorArray[number]->GetActorLocation();
	}

	Character = ReviveActor;

	if (Character)
	{
		GetWorld()->GetTimerManager().SetTimer(ReviveTimer, [this]() 
		{
			bIsDead = false;
			Character->ChangeViewCamera(true);
			DeathNum = EKindOfDeath::EKOD_NONE;
			Character->GetCapsuleComponent()->SetCollisionProfileName("Alive");
			Character->GetMesh()->SetCollisionProfileName("CharacterMesh");

			if (Character->GetFPSCharacterStatComponent())
			{
				Character->GetFPSCharacterStatComponent()->InitCharacterHP();
				Character->GetFPSCharacterStatComponent()->PurchaseKavlarAndHelmet();
			}

			if (Character->GetPlayerController())
			{
				if (Character->GetPlayerController()->IsLocalController())
				{
					if (Character->GetFPSUIWidget())
					{
						Character->GetFPSUIWidget()->SetArmorAndHealth(Character);
						Character->GetFPSCharacterStatComponent()->GetCurrentWeapon()->Draw(Character);
						if (Character->GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
						{
							Character->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->InitAmmoCount();
						}
						Character->GetFPSUIWidget()->SetAmmoCount(Character);
					}
				}
			}


			Character->SetActorLocation(RevivePosition);

		}, ReviveSeconds, false);
	}


}

void UFPSCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSCharacterStatComponent, WeaponArray);
	DOREPLIFETIME(UFPSCharacterStatComponent, CurrentHP);
	DOREPLIFETIME(UFPSCharacterStatComponent, CurrentKevlar);
	DOREPLIFETIME(UFPSCharacterStatComponent, CurrentDollar);
	DOREPLIFETIME(UFPSCharacterStatComponent, DeathNum);
	DOREPLIFETIME(UFPSCharacterStatComponent, KillCount);
}
