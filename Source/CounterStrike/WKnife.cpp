// Fill out your copyright notice in the Description page of Project Settings.

#include "WKnife.h"
#include "DrawDebugHelpers.h"
#include "UObject/NameTypes.h"
#include "GameFramework/Controller.h"
#include "FPSCharacterStatComponent.h"
#include "ActorPool.h"
#include "Engine/DecalActor.h"

#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"

AWKnife::AWKnife()
{
	eWeaponNum = EWeaponNum::E_Knife;
	bCanAutoFire = true;

	SocketName = TEXT("Knife_Socket");

	RunSpeedRatio = 1.0f;
}

void AWKnife::Fire()
{
	Super::Fire();
}

void AWKnife::Action()
{
	if (Player) 
	{
		Player->AttackAnimCall = Player->IsActionHeld;
		//CheckAttack();
		CurrentPlayingAnim = ActionAnim;
		PlayAnim(false);
		Super::Action();
	}
}

void AWKnife::CheckAttack()
{
	if (Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber() != eWeaponNum) return;

	FVector Location;
	FRotator Rotation;

	// Create Ray..
	Player->GetController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	TArray<FVector> vec;

	FVector Direction = Player->FPSCameraComponent->GetForwardVector() * Weapondistance;

	Location += Player->FPSCameraComponent->GetForwardVector() * 25.f;

	// Middle -> Left -> Right
	vec.Add(Location + Direction);
	vec.Add(Location + Direction.RotateAngleAxis(-10.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(-20.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(-30.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(-40.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(10.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(20.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(30.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction.RotateAngleAxis(40.f, FVector(Player->FPSCameraComponent->GetUpVector())));
	vec.Add(Location + Direction);


	FHitResult Hit;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player);

	KnifeAttackDirection.Empty();

	switch (Player->GetFPSCharacterStatComponent()->GetCharacterUpperState())
	{
		case UPPER_STATE::ACTION:
			for (int i = 9; i >= 0; --i)
			{
				GetWorld()->LineTraceSingleByChannel(Hit, Location, vec[i], ECollisionChannel::ECC_Visibility, CollisionParams);

				if (Hit.GetActor())
				{
					if (HitResults.Num() > 0)
					{
						bool flag = false;
						for (int j = 0; j < HitResults.Num(); ++j)
						{
							if (HitResults[j] == Hit.GetActor())
							{
								flag = true;
							}
						}
						if (!flag)
						{
							HitResults.Add(Hit.GetActor());
							HitPoint.Add(Hit);
							KnifeAttackDirection.Add(vec[i] - Location);
						}
					}

					else
					{
						HitResults.Add(Hit.GetActor());
						HitPoint.Add(Hit);
						KnifeAttackDirection.Add(vec[i] - Location);
					}
				}

				//DrawDebugLine(GetWorld(), Location, vec[i], FColor::Blue, true, 2.f, 0, 2.f);
			}

			if (HitPoint.Num() > 0)
			{
				Player->FPSmesh[uint8(Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber()) - 1]->PlayAnimation(ActionHitAnim, false);
			}
			break;

		case UPPER_STATE::ATTACK:
			for (int i = 0; i < 10; ++i)
			{
				GetWorld()->LineTraceSingleByChannel(Hit, Location, vec[i], ECollisionChannel::ECC_Visibility, CollisionParams);

				if (Hit.GetActor())
				{
					if (HitResults.Num() > 0)
					{
						bool flag = false;
						for (int j = 0; j < HitResults.Num(); ++j)
						{
							if (HitResults[j] == Hit.GetActor())
							{
								flag = true;
							}
						}
						if (!flag)
						{
							HitResults.Add(Hit.GetActor());
							HitPoint.Add(Hit);
							KnifeAttackDirection.Add(vec[i] - Location);
						}
					}

					else
					{
						HitResults.Add(Hit.GetActor());
						HitPoint.Add(Hit);
						KnifeAttackDirection.Add(vec[i] - Location);
					}
				}

				//DrawDebugLine(GetWorld(), Location, vec[i], FColor::Blue, true, 2.f, 0, 2.f);
			}
			break;
	}
}

void AWKnife::SetHit()
{
	if (Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber() != eWeaponNum) return;

	//uint8 PointCount = 0;

	for (int j = 0; j < HitPoint.Num(); j++)
	{
		if (HitPoint[j].GetActor() && HitPoint[j].GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *HitPoint[j].GetActor()->GetName());

			AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(HitPoint[j].GetActor());
			if (DamagedCharacter)
			{
				bool Check = CheckBackAttack(DamagedCharacter);
				SpawnDecal(HitPoint[j], EDecalPoolList::EDP_BLOOD);

				switch (Player->GetFPSCharacterStatComponent()->GetCharacterUpperState())
				{
				case UPPER_STATE::ATTACK:
					UGameplayStatics::SpawnSoundAttached(HitSound, Player->GetMesh(), SocketName);
					if (Check)
					{
						if (ActorPool)
						{
							DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, 90, Penetration, EBoneHit::EB_NONE, HitPoint[j].ImpactPoint, ActorPool->GetBloodParticle(), KnifeAttackDirection[j].GetSafeNormal());
						}
					}
					else
					{
						if (ActorPool)
						{
							DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, 40, Penetration, EBoneHit::EB_NONE, HitPoint[j].ImpactPoint, ActorPool->GetBloodParticle(), KnifeAttackDirection[j].GetSafeNormal());
						}
					}

					break;
				case UPPER_STATE::ACTION:
					UGameplayStatics::SpawnSoundAttached(StabSound, Player->GetMesh(), SocketName);
					if (Check)
					{
						if (ActorPool)
						{
							DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, 180, Penetration, EBoneHit::EB_NONE, HitPoint[j].ImpactPoint, ActorPool->GetBloodParticle(), KnifeAttackDirection[j].GetSafeNormal());
						}
					}
					else
					{
						if (ActorPool)
						{
							DamagedCharacter->GetFPSCharacterStatComponent()->GetDamage(DamagedCharacter, 65, Penetration, EBoneHit::EB_NONE, HitPoint[j].ImpactPoint, ActorPool->GetBloodParticle(), KnifeAttackDirection[j].GetSafeNormal());
						}
					}

					break;
				}
			}
		}

		else
		{
			if (j == 0)
			{
				UGameplayStatics::SpawnSoundAttached(HitWallSound, Player->GetMesh(), SocketName);
				SpawnDecal(HitPoint[0], EDecalPoolList::EDP_SCAR);
			}
		}
	}


	HitResults.Empty();
	HitPoint.Empty();
}

void AWKnife::ShuffleShotAnim()
{
	uint8 random = FMath::RandRange(0, 2);

	switch (random)
	{
	case 1:
		CurrentPlayingAnim = AttackAnim_2;
		break;
	default:
		CurrentPlayingAnim = AttackAnim;
		break;
	}

	Super::ShuffleShotAnim();
}

void AWKnife::SpawnDecal(FHitResult Hit, EDecalPoolList Type)
{
	if (!ActorPool)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorPool::StaticClass(), actors);
		ActorPool = Cast<AActorPool>(actors[0]);
	}

	if (ActorPool)
	{
		ADecalActor* Decal = nullptr;

		switch (Type)
		{
		case EDecalPoolList::EDP_SCAR:
			Decal = ActorPool->GetDecal(EDecalPoolList::EDP_SCAR);
			if (Decal)
			{
				Decal->SetActorLocation(Hit.ImpactPoint);
				Decal->SetActorRotation(Hit.ImpactNormal.Rotation());
				Decal->SetActorHiddenInGame(false);
			}
			break;

		case EDecalPoolList::EDP_BLOOD:
			Decal = ActorPool->GetDecal(EDecalPoolList::EDP_BLOOD);

			if (Decal)
			{
				FHitResult PenetrationResult = CheckWall(Hit, Player->FPSCameraComponent->GetForwardVector() * 150.f, true);

				if (PenetrationResult.GetActor())
				{
					Decal->SetActorLocation(PenetrationResult.ImpactPoint);
					Decal->SetActorRotation(PenetrationResult.ImpactNormal.Rotation());
					Decal->SetActorHiddenInGame(false);
				}

				else
				{
					PenetrationResult = CheckWall(Hit, -Hit.GetActor()->GetActorUpVector() * 1000.f, true);
					if (PenetrationResult.GetActor())
					{
						FVector Vec = FVector(FMath::RandRange(PenetrationResult.ImpactPoint.X - 50.f, PenetrationResult.ImpactPoint.X + 50.f),
							FMath::RandRange(PenetrationResult.ImpactPoint.Y - 50.f, PenetrationResult.ImpactPoint.Y + 50.f), PenetrationResult.ImpactPoint.Z);
						Decal->SetActorLocation(Vec);
						Decal->SetActorRotation(PenetrationResult.ImpactNormal.Rotation());
						Decal->SetActorHiddenInGame(false);
					}
				}
			}
			break;
		}
	}
}

bool AWKnife::CheckBackAttack(AFPSCharacter* Actor)
{

	float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Player->FPSCameraComponent->GetForwardVector(),
		Actor->FPSCameraComponent->GetForwardVector())));

	//UE_LOG(LogTemp, Warning, TEXT("%.1f"), Angle);

	if (Angle >= 90.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack!!!"));
		return false;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BackAttack!!!"));
		return true;
	}
}

void AWKnife::BeginPlay()
{
	Super::BeginPlay();
}
