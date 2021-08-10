// Fill out your copyright notice in the Description page of Project Settings.

#include "WBase.h"

#include "DrawDebugHelpers.h"
#include "WKnife.h"
#include "WRifle.h"
#include "WSniperRifle.h"
#include "FPSCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActorPool.h"
#include "Engine/DecalActor.h"

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
	FPSHandComponent->SetVisibility(false);

	TPSWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("TPSWeaponComp");
	TPSWeaponComponent->SetVisibility(false);

	TPSWeaponComponent->SetRelativeScale3D(FVector(3, 3, 3));

	WaitTime = 0.f;

	WeaponComponent->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WeaponComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	WeaponComponent->SetWorldScale3D(FVector(3, 3, 3));
	WeaponComponent->CanCharacterStepUp(false);

	WeaponComponent->SetCollisionProfileName(TEXT("Weapon"));
	WeaponComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponComponent->SetMassOverrideInKg(TEXT(""), 500);
	WeaponComponent->SetSimulatePhysics(false);

	//bReplicates = true;
}

void AWBase::PlayAnim(bool loop)
{
	if (CurrentPlayingAnim && Player)
	{
		Player->FPSmesh[uint8(Player->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber()) - 1]->PlayAnimation(CurrentPlayingAnim, loop);
	}
}

float AWBase::GetRunSpeed()
{
	if (Player && Player->GetFPSCharacterStatComponent())
	{

		if (Player->IsWalkHeld)
		{
			return Player->GetFPSCharacterStatComponent()->WalkSpeed;
		}

		if (Player->IsCrouchHeld)
		{
			return Player->GetFPSCharacterStatComponent()->CrouchSpeed;
		}

		if(!Player->IsCrouchHeld && !Player->IsWalkHeld)
		{
			return Player->GetFPSCharacterStatComponent()->SprintSpeed * RunSpeedRatio;
		}
	}

	return 600.f * RunSpeedRatio;
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
		Player->DelayTime = GetAttackDelay();

		ShuffleShotAnim();

		if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
		{
			Player->SyncClientAttack(Player->AttackAnimCall, Player->DelayTime);
		}

		if (!bCanAutoFire)
		{
			GetWorld()->GetTimerManager().SetTimer(SingleShootHandle, [this]() {

				Player->AttackAnimCall = false;

				if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
				{
					Player->SyncClientAttack(Player->AttackAnimCall, Player->DelayTime);
				}

			}, 0.1f, false);
		}

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {

			// 타이머 초기화
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			// 애니메이션 공격을 멈추도록 false
			Player->AttackAnimCall = false;
			// RPC 함수를 호출 다른 클라이언트들에게 보이는 공격 애니메이션 설정
			Player->SyncClientAttack(Player->AttackAnimCall, Player->DelayTime);

			// 만약 마우스를 클릭하고 있다면...
			if (Player->IsAttackHeld)
			{
				// 연사가 가능한 무기라면..
				if (bCanAutoFire)
				{
					// 총을 들고 있고, 총의 현재탄약이 0이라면...
					if ((eWeaponNum == EWeaponNum::E_Rifle ||
						eWeaponNum == EWeaponNum::E_Sub) &&
						Player->GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->GetCurrentAmmoCount() == 0)
					{
						// 현재 재생하고있는 애니메이션 길이와 무기 공격 후 딜레이를 빼서 Idle 애니메이션이 자연스럽게 이어지도록 WaitTime을 설정
						WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
						Idle();
					}

					else
					{
						Fire();
					}
				}

				// 아닌 경우..
				else
				{
					StopFire();
					WaitTime = CurrentPlayingAnim->SequenceLength - GetAttackDelay();
					Idle();
				}
			}

			// 클릭을 하지 않고 있다면...
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
				if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
				{
					Player->SyncClientAttack(false, 0.f);
				}
			}

		}, GetActionDelayTime(), false);

	}
}

void AWBase::StopFire()
{
	Player->IsAttackHeld = false;
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
	}

	if (!Player)
		return;

	StopFire();
	StopAction();
	Player->bIsReloading = false;


	Player->GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();

	Player->CurrentAnimationWeaponNumber = uint8(eGunNumber);

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


	if (Player->GetLocalRole() < ROLE_Authority && Player->IsLocallyControlled())
	{
		Player->SyncClientAttack(false, 0.f);
		Player->SyncClientSpeed(GetRunSpeed());
	}

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


AActorPool* AWBase::GetActorPool()
{
	if (!ActorPool)
	{
		ActorPool = Cast<AActorPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorPool::StaticClass()));
	}

	return ActorPool;
}

bool AWBase::GetPossiblePickWeapon()
{
	if (bPossiblePick)
	{
		switch (eWeaponNum)
		{
		case EWeaponNum::E_Rifle:
		case EWeaponNum::E_Sub:
		case EWeaponNum::E_C4:
		case EWeaponNum::E_Knife:
			return true;

		case EWeaponNum::E_Grenade:
		case EWeaponNum::E_Smoke:
		case EWeaponNum::E_Flash:
			return false;
		default:
			return false;
		}
	}

	else
	{
		return false;
	}
}
