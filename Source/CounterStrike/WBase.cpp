// Fill out your copyright notice in the Description page of Project Settings.

#include "WBase.h"
#include "WGun.h"
#include "WKnife.h"
#include "WRifle.h"
#include "WSniperRifle.h"
#include "FPSCharacterStatComponent.h"


// Sets default values
AWBase::AWBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(Root);

	PrimaryActorTick.bCanEverTick = false;
	WeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = WeaponComponent;
	//WeaponComponent->SetEnableGravity(true);

	FPSHandComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FPSHandComp");
	//FPSHandComponent->SetupAttachment(Root);

	WaitTime = 0.f;

	WeaponComponent->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WeaponComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	WeaponComponent->SetWorldScale3D(FVector(3, 3, 3));
	WeaponComponent->CanCharacterStepUp(false);
}

void AWBase::PlayAnim(bool loop)
{
	if (CurrentPlayingAnim && Player)
	{
		Player->FPSmesh[uint8(Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber()) - 1]->PlayAnimation(CurrentPlayingAnim, loop);
	}
}

// Called when the game starts or when spawned
void AWBase::BeginPlay()
{
	Super::BeginPlay();
}

void AWBase::ShuffleShotAnim()
{
	PlayAnim(false);
}

float AWBase::GetAnimAttackEndTime()
{
	if (CurrentPlayingAnim)
	{
		return CurrentPlayingAnim->SequenceLength;
	}

	return 0.f;
}

void AWBase::ResetTimeHandle()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}

	if (GetWorldTimerManager().IsTimerActive(SingleShootHandle))
	{
		GetWorldTimerManager().ClearTimer(SingleShootHandle);
	}
}


void AWBase::Idle()
{
	if (Player)
	{
		if (Player->GetFPSCharacterStatComponent()->GetCharacterUpperState() == UPPER_STATE::IDLE)
		{
			return;
		}

		CurrentPlayingAnim = IdleAnim;
		Player->SetUpperState(UPPER_STATE::IDLE);


		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			PlayAnim(true);

		}, WaitTime, false);

		if (Player->GetFPSCharacterStatComponent()->GetCurrentWeapon()->IsA(AWGun::StaticClass()))
		{
			if (Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetCurrentAmmoCount() == 0)
			{
				Player->AttackAnimCall = false;
				Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->Reload();
				return;
			}
		}
	}
}

void AWBase::Fire()
{
	if (Player) 
	{
		Player->AttackAnimCall = true;
		Player->SetUpperState(UPPER_STATE::ATTACK);
		Player->DelayTime = Player->GetFPSCharacterStatComponent()->GetCurrentWeapon()->GetAttackDelay();

		ShuffleShotAnim();

		if (!bCanAutoFire)
		{
			GetWorld()->GetTimerManager().SetTimer(SingleShootHandle, [this]() {

				Player->AttackAnimCall = false;

			}, 0.15f, false);
		}

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			Player->AttackAnimCall = false;

			if (Player->IsAttackHeld)
			{
				if (bCanAutoFire)
				{
					if ((eWeaponNum == EWeaponNum::E_Rifle ||
						eWeaponNum == EWeaponNum::E_Sub) &&
						Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetCurrentAmmoCount() == 0)
					{
						WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
						Idle();
					}

					else
					{
						Fire();
					}
				}

				else
				{
					StopFire();
					WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
					Idle();
				}
			}

			else
			{
				WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
				Idle();
			}

		}, GetAttackDelay(), false);
	}
}

void AWBase::Action()
{
	if (Player) 
	{
		Player->DelayTime = Player->GetFPSCharacterStatComponent()->GetCurrentWeapon()->GetActionDelayTime();

		Player->SetUpperState(UPPER_STATE::ACTION);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			if (Player->IsActionHeld)
			{
				Action();
			}

			else 
			{
				WaitTime = CurrentPlayingAnim->SequenceLength - GetActionDelayTime();
				Idle();
			}

		}, GetActionDelayTime(), false);

	}
}

void AWBase::StopFire()
{
	Player->IsAttackHeld = false;
	//Player->AttackAnimCall = Player->IsAttackHeld;
}

void AWBase::StopAction()
{
	Player->IsActionHeld = false;
	Player->AttackAnimCall = Player->IsActionHeld;
}

float AWBase::GetDrawDelay()
{
	return DrawAnim->SequenceLength;
}


void AWBase::Draw(AFPSCharacter* player)
{
	
	if (!Player || Player != player)
	{
		Player = player;
		TakeWeaponRunSpeed = player->GetFPSCharacterMovement()->GetRunSpeed() * RunSpeedRatio;
	}

	if (!Player)
		return;

	StopFire();
	StopAction();
	Player->bIsReloading = false;

	Player->GetFPSCharacterMovement()->SetRunSpeed(TakeWeaponRunSpeed);

	CurrentPlayingAnim = DrawAnim;
	// Play Anim...	
	PlayAnim(false);
	Player->SetUpperState(UPPER_STATE::DRAW);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

		if (Player->IsAttackHeld)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			Fire();
		}

		else if (Player->IsActionHeld)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			Action();
		}

		else {
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			Idle();
		}

	}, GetDrawDelay(), false);
}



FHitResult AWBase::CheckWall(FHitResult StartPoint, FVector Direction, bool bIgnoreMyself)
{
	FHitResult Hit;

	FVector End2;

	FCollisionQueryParams CollisionParams;
	if (bIgnoreMyself)
	{
		CollisionParams.AddIgnoredActor(StartPoint.GetActor());
	}
	CollisionParams.AddIgnoredActor(Player);


	End2 = StartPoint.ImpactPoint + Direction;

	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
		StartPoint.ImpactPoint, End2,
		ECollisionChannel::ECC_WorldStatic, CollisionParams);

	if (Hit.GetActor())
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s "), *Hit[i].GetActor()->GetName());
		if (!Hit.GetActor()->IsA(AFPSCharacter::StaticClass()))
		{
			return Hit;
		}
	}

	return FHitResult();
}