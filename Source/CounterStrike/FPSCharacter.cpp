// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacter.h"
#include "Math/Vector.h"

#include "Global.h"
#include "WBase.h"
#include "WGun.h"
#include "WKnife.h"
#include "WSniperRifle.h"

#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "FPSCharacterStatComponent.h"
#include "CounterStrikeGameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Controller.h"
#include "FPSCharacterAnimInstance.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

#include "ActorPool.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

#include "FPSHUDWidget.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/PlayerState.h"
#include "ActorPool.h"
#include "Animation/SkeletalMeshActor.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(55.f, 80.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	GetCapsuleComponent()->bReturnMaterialOnMove = true;

	//set our turn rates for input
	BaseTurnRate = 30.0f;
	BaseLookUpRate = 30.0f;
	BaseEyeHeight = 65.0f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 0.f;
	SpringArmComponent->bDoCollisionTest = false;

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->SetupAttachment(SpringArmComponent);
	FPSCameraComponent->bUsePawnControlRotation = true;
	//FPSCameraComponent->SetIsReplicated(true);

	StatComponent = CreateDefaultSubobject<UFPSCharacterStatComponent>(TEXT("CharacterStatComponent"));

	//CaptureCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureCamera"));
	//CaptureCamera->SetRelativeLocation(FVector(0, 0, 100.f));
	//CaptureCamera->SetRelativeRotation(FRotator(-90.0f, 0, 0));
	//CaptureCamera->ProjectionType = ECameraProjectionMode::Orthographic;

	//CaptureCamera->SetupAttachment(RootComponent);

	//CaptureCamera->HideComponent(GetMesh());

	for (int i = 0; i < HANDCOUNT; ++i)
	{
		USkeletalMeshComponent* HandMesh;

		FName MeshName = FName(*FString::Printf(TEXT("FPSMesh_%d"), i + 1));
		HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(MeshName);
		HandMesh->SetupAttachment(FPSCameraComponent);
		HandMesh->SetCastShadow(false);

		HandMesh->SetOnlyOwnerSee(true);
		HandMesh->SetOwnerNoSee(false);

		HandMesh->SetRelativeLocation(FVector(-5.f, 0, 0.f));
		HandMesh->SetRelativeRotation(FRotator(0, -90, 0));
		HandMesh->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));


		HandMesh->SetVisibility(false);
		//CaptureCamera->HideComponent(HandMesh);
		FPSmesh.Add(HandMesh);

	}


	for (int i = 0; i < HANDCOUNT; ++i)
	{
		USkeletalMeshComponent* WeaponMesh;

		FName MeshName = FName(*FString::Printf(TEXT("WeaponMesh_%d"), i + 1));
		WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(MeshName);
		WeaponMesh->SetupAttachment(GetMesh());
		WeaponMesh->SetOwnerNoSee(true);
		WeaponMesh->SetVisibility(false);
		WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
		WeaponMesh->CanCharacterStepUp(false);
		//WeaponMesh->SetIsReplicated(true);
		Thirdmesh.Add(WeaponMesh);

		//CaptureCamera->HideComponent(WeaponMesh);
	}

	GetMesh()->SetOwnerNoSee(true);

	DecBEH = BaseEyeHeight;
	DecCapsHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	//PlayerMovement = GetFPSCharacterMovement();

	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	//GetCapsuleComponent()->SetIsReplicated(true);

	GetMesh()->SetRelativeLocation(FVector(0, 0, 50));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));
	GetMesh()->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

	//GetMesh()->SetIsReplicated(true);

	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 0, 65));

	MeshHeight = GetMesh()->GetRelativeLocation().Z;

	GetCharacterMovement()->CrouchedHalfHeight = 60.f;
	GetCharacterMovement()->JumpZVelocity = 500.f;




	//static ConstructorHelpers::FClassFinder<UFPSHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/BluePrint/HUD/InGame/FPSUI.FPSUI_C'"));
	//if (UI_HUD_C.Succeeded())
	//{
	//	HUDWidgetClass = UI_HUD_C.Class;
	//}

	GetCapsuleComponent()->SetCollisionProfileName("Alive");

	SetReplicates(true);
	
	//GetPlayerController()->GetPawn()->bUseControllerRotationYaw = false;
	if (StatComponent)
	{
		GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
		GetCharacterMovement()->MaxWalkSpeedCrouched = StatComponent->CrouchSpeed;
	}

 }

 void AFPSCharacter::SetFPSUIHUD(APlayerController* MyController)
 {
	 if (HUDWidgetClass)
	 {
		 HUDWidget = CreateWidget(MyController, HUDWidgetClass);
		 if (HUDWidget)
		 {
			 HUDWidget->AddToViewport(1);
		 }
		 else
		 {
			 GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Not HUDWidgetClass"));
		 }
	 }

	 FPSUIWidget = Cast<UFPSHUDWidget>(HUDWidget);
	 if (FPSUIWidget)
	 {
		 FPSUIWidget->Init(this);
		 //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Init HUD %s"), *GetPlayerState()->GetPlayerName()));
	 }
 }


 // Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//UGameplayStatics::SpawnSoundAttached(StartSound, GetMesh());

	IsAttackHeld = false;
	IsActionHeld = false;

	Instance = GetMesh()->GetAnimInstance();

	if (Instance)
	{
		animInstance = Cast<UFPSCharacterAnimInstance>(Instance);

		if (animInstance)
		{
			animInstance->GetPlayer(this);
			//UE_LOG(LogTemp, Warning, TEXT("Get Player!"));
		}
	}

	ChangeViewCamera(false);

}


// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//SetCharacterState();

	/*if (StatComponent)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("UpperState : ") + GetStateAsString(StatComponent->GetCharacterUpperState()));
		GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, TEXT("LowerState : ") + GetStateAsString(StatComponent->GetCharacterLowerState()));
		GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Yellow, FString::Printf(TEXT("Velocity : %.1f"), GetMovementComponent()->Velocity.Size2D()));
	}*/


	RotatingLowerHips(DeltaTime);
	ResetLowerHips(DeltaTime);
	SmoothingCrouch(DeltaTime);
	ShakeHand(DeltaTime);
	

	if (FPSUIWidget)
	{
		FPSUIWidget->DynamicCrosshair(this, DeltaTime);
		if (bIsFlashBang)
		{
			FPSUIWidget->FlashBang(this, DeltaTime);
		}
	}


	if (IsJumpHeld)
	{
		Jump();
	}

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	check(InputComponent);
	// "movement" 바인딩을 구성합니다.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// "look" 바인딩을 구성합니다.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// "action" 바인딩을 구성합니다.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSCharacter::StopCrouch);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AFPSCharacter::StartWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AFPSCharacter::StopWalk);

	//PlayerInputComponent->BindAction("ViewChange", IE_Pressed, this, &AFPSCharacter::ChangeViewCamera);

	PlayerInputComponent->BindAction("Shot", IE_Pressed, this, &AFPSCharacter::Shot);
	PlayerInputComponent->BindAction("Shot", IE_Released, this, &AFPSCharacter::StopShot);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AFPSCharacter::Action);
	PlayerInputComponent->BindAction("Action", IE_Released, this, &AFPSCharacter::StopAction);

	//PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AFPSCharacter::Interaction);

	PlayerInputComponent->BindAction("Swap_Main", IE_Pressed, this, &AFPSCharacter::Swap_Main);
	PlayerInputComponent->BindAction("Swap_Sub", IE_Pressed, this, &AFPSCharacter::Swap_Sub);
	PlayerInputComponent->BindAction("Swap_Knife", IE_Pressed, this, &AFPSCharacter::Swap_Knife);
	PlayerInputComponent->BindAction("Swap_Granade", IE_Pressed, this, &AFPSCharacter::Swap_Granade);
	PlayerInputComponent->BindAction("Swap_Bomb", IE_Pressed, this, &AFPSCharacter::Swap_Bomb);

	//PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AFPSCharacter::Drop);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::Reload);

	//PlayerInputComponent->BindAction("Shop", IE_Pressed, this, &AFPSCharacter::Shop);

	//PlayerInputComponent->BindAction("Check", IE_Pressed, this, &AFPSCharacter::CheckMyWeapon);
}

void AFPSCharacter::SetRagdoll()
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
}

void AFPSCharacter::Shop()
{
	//PurchaseWeapon(ECreatWeaponNum::EC_AK);
}

void AFPSCharacter::Shot()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}

		if (!IsPossibleAttack)
			return;
	}

	if (StatComponent->GetCurrentWeapon())
	{
		IsAttackHeld = true;
		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::IDLE)
		{
			StatComponent->GetCurrentWeapon()->Fire();
		}
	}
}

void AFPSCharacter::StopShot()
{
	if (!StatComponent || !IsAttackHeld)
		return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (StatComponent->GetCurrentWeapon())
	{
		StatComponent->GetCurrentWeapon()->StopFire();
	}
}

void AFPSCharacter::SyncClientAttack_Implementation(bool flag, float Time)
{
	AttackAnimCall = flag;
	DelayTime = Time;

}


void AFPSCharacter::Action()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
		if (!IsPossibleAttack)
			return;
	}


	if (StatComponent->GetCurrentWeapon())
	{
		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::IDLE)
		{
			IsActionHeld = true;
			StatComponent->GetCurrentWeapon()->Action();
		}
	}
}

void AFPSCharacter::StopAction()
{

	if (!StatComponent || !IsActionHeld )
		return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (StatComponent->GetCurrentWeapon())
	{
		StatComponent->GetCurrentWeapon()->StopAction();
	}
}

void AFPSCharacter::Drop()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	DropWeapon(this->GetFPSCharacterStatComponent()->GetCurrentWeaponNumber());
}

void AFPSCharacter::Reload()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}


	if (StatComponent->GetCurrentWeapon())
	{
		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::IDLE)
		{
			if (StatComponent->GetCurrentGunWeapon())
			{
				StatComponent->GetCurrentGunWeapon()->Reload();
			}
		}
	}
}

void AFPSCharacter::Interaction()
{
	if (!StatComponent) 
		return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	FVector Location;
	FRotator Rotation;

	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector End = Location + Rotation.Vector() * InteractionDistance;

	
	FHitResult Hit;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_Visibility, CollisionParams);

	if (bSucess)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s"), *Hit.Actor->GetName()));
		TakeWeapon(Hit);
	}
}

void AFPSCharacter::Swap_Main()
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	ChangeWeapon(EWeaponNum::E_Rifle);
}

void AFPSCharacter::Swap_Sub()
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	ChangeWeapon(EWeaponNum::E_Sub);
}

void AFPSCharacter::Swap_Knife()
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	ChangeWeapon(EWeaponNum::E_Knife);

}

void AFPSCharacter::Swap_Granade()
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	switch (CurrentGrenade)
	{
	case EWeaponNum::E_Grenade:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Smoke))
		{
			ChangeWeapon(EWeaponNum::E_Smoke);
			CurrentGrenade = EWeaponNum::E_Smoke;
		}

		else if (StatComponent->GetSelectWeapon(EWeaponNum::E_Flash))
		{
			ChangeWeapon(EWeaponNum::E_Flash);
			CurrentGrenade = EWeaponNum::E_Flash;
		}

		else
		{
			return;
		}

		break;
	case EWeaponNum::E_Smoke:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Flash))
		{
			ChangeWeapon(EWeaponNum::E_Flash);
			CurrentGrenade = EWeaponNum::E_Flash;
		}

		else if (StatComponent->GetSelectWeapon(EWeaponNum::E_Grenade))
		{
			ChangeWeapon(EWeaponNum::E_Grenade);
			CurrentGrenade = EWeaponNum::E_Grenade;
		}

		else
		{
			return;
		}
		break;
	case EWeaponNum::E_Flash:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Grenade))
		{
			ChangeWeapon(EWeaponNum::E_Grenade);
			CurrentGrenade = EWeaponNum::E_Grenade;
		}

		else if (StatComponent->GetSelectWeapon(EWeaponNum::E_Smoke))
		{
			ChangeWeapon(EWeaponNum::E_Smoke);
			CurrentGrenade = EWeaponNum::E_Smoke;
		}

		else
		{
			return;
		}
		break;
	default:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Grenade))
		{
			ChangeWeapon(EWeaponNum::E_Grenade);
			CurrentGrenade = EWeaponNum::E_Grenade;
		}

		else if (StatComponent->GetSelectWeapon(EWeaponNum::E_Smoke))
		{
			ChangeWeapon(EWeaponNum::E_Smoke);
			CurrentGrenade = EWeaponNum::E_Smoke;
		}

		else if(StatComponent->GetSelectWeapon(EWeaponNum::E_Flash))
		{
			ChangeWeapon(EWeaponNum::E_Flash);
			CurrentGrenade = EWeaponNum::E_Flash;
		}

		else
		{
			return;
		}
		break;
	}

}

void AFPSCharacter::Swap_Bomb()
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	ChangeWeapon(EWeaponNum::E_C4);
}

float AFPSCharacter::CaculatingDot(FVector Vector1, FVector Vector2)
{
	//UE_LOG(LogTemp, Warning, TEXT("%.1f"), FVector::DotProduct(GetActorForwardVector(), FPSCameraComponent->GetForwardVector()));

	return FVector::DotProduct(Vector1, Vector2);
	//return FVector::Product(GetActorForwardVector(), FPSCameraComponent->GetForwardVector());
}

void AFPSCharacter::ShakeHand(float DeltaTime)
{

	if (!StatComponent)
		return;

	if (!StatComponent->GetIsDead() && StatComponent->GetCurrentWeapon())
	{
		if (!FMath::IsNearlyZero(GetMovementComponent()->Velocity.Size2D()))
		{
			FVector vec = GetCurrentFPSMesh()->GetRelativeLocation();
			runningTime += DeltaTime * 5.f;

			// Modify..

			if (!IsWalkHeld)
			{
				vec.X = FMath::Sin(runningTime) * 1.5f;
			}

			else {
				vec.X = FMath::Sin(runningTime) * 0.7f;
			}

			GetCurrentFPSMesh()->SetRelativeLocation(vec);
		}

		else if ((FMath::IsNearlyZero(MoveForwardValue) &&
			FMath::IsNearlyZero(MoveRightValue) &&
			FMath::IsNearlyZero(GetMovementComponent()->Velocity.Size2D()))
			|| StatComponent->GetCharacterUpperState() != UPPER_STATE::IDLE)
		{
			runningTime = 0;
			GetCurrentFPSMesh()->SetRelativeLocation(
				FMath::VInterpTo(GetCurrentFPSMesh()->GetRelativeLocation(), FVector(0, 0, 0), DeltaTime, 20));
		}
	}
}

//void AFPSCharacter::SetCharacterState()
//{
//
//	// Modify...
//
//	if (GetMovementComponent()->IsFalling())
//	{
//		StatComponent->SetCharacterLowerState(LOWER_STATE::JUMP);
//	}
//
//	else
//	{
//		if (IsCrouchHeld)
//		{
//			StatComponent->SetCharacterLowerState(LOWER_STATE::CROUCH);
//		}
//		else
//		{
//			StatComponent->SetCharacterLowerState(LOWER_STATE::IDLE);
//		}
//	}
//
//}

TSubclassOf<AWBase> AFPSCharacter::GetWeaponBP(ECreatWeaponNum Number)
{
	TSubclassOf<AWBase> SpawnWeapon = nullptr;

	if (AActorPool* Pool = GetActorPool())
	{
		switch (Number)
		{
		case ECreatWeaponNum::EC_Knife:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Knife.BP_Knife_C'";
			SpawnWeapon = Pool->GetKnife();
			break;
		case ECreatWeaponNum::EC_AK:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_AK47.BP_AK47_C'";
			SpawnWeapon = Pool->GetAK47();
			break;
		case ECreatWeaponNum::EC_AWP:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Awp.BP_Awp_C'";
			SpawnWeapon = Pool->GetAWP();
			break;
		case ECreatWeaponNum::EC_DEAGLE:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Deagle.BP_Deagle_C'";
			SpawnWeapon = Pool->GetDeagle();
			break;
		case ECreatWeaponNum::EC_GLOCK:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Glock18.BP_Glock18_C'";
			SpawnWeapon = Pool->GetGlock18();
			break;
		case ECreatWeaponNum::EC_GRENADE:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Grenade.BP_Grenade_C'";
			SpawnWeapon = Pool->GetGrenade();
			break;
		case ECreatWeaponNum::EC_SMOKE:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Smoke.BP_Smoke_C'";
			SpawnWeapon = Pool->GetSmoke();
			break;
		case ECreatWeaponNum::EC_FLASH:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Flash.BP_Flash_C'";
			SpawnWeapon = Pool->GetFlash();
			break;
		case ECreatWeaponNum::EC_M4A1:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_M4A1.BP_M4A1_C'";
			SpawnWeapon = Pool->GetM4A1();
			break;
		case ECreatWeaponNum::EC_MAC10:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Mac10.BP_Mac10_C'";
			SpawnWeapon = Pool->GetMac10();
			break;
		case ECreatWeaponNum::EC_NOVA:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Nova.BP_Nova_C'";
			SpawnWeapon = Pool->GetNova();
			break;
		case ECreatWeaponNum::EC_SCOUT:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Scout.BP_Scout_C'";
			SpawnWeapon = Pool->GetScout();
			break;
		case ECreatWeaponNum::EC_USP:
			//Path = "Blueprint'/Game/BluePrint/Weapon/BP_Usp.BP_Usp_C'";
			SpawnWeapon = Pool->GetUSP();
			break;
		}
	}

	return SpawnWeapon;
}

AWBase* AFPSCharacter::FindWeapon(EWeaponNum FindWeaponNumber)
{
	// if find Weapon return True, else return False...

	if (!StatComponent) return nullptr;

	if (StatComponent->GetWeaponArrayNum() == 0)
		return nullptr;
	
	else 
	{
		return StatComponent->GetSelectWeapon(FindWeaponNumber);
	}
}

void AFPSCharacter::ChangeWeapon(const EWeaponNum ChangeWeaponNumber)
{

	//if (!StatComponent) return;

	//AWBase* Weapon = FindWeapon(ChangeWeaponNumber);

	//if (!Weapon)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Not Found Any Weapon!!!"));
	//	return;
	//}

	//if (StatComponent->GetCurrentWeapon())
	//{
	//	if (StatComponent->GetCurrentWeapon() == Weapon)
	//	{
	//		return;
	//	}

	//	else 
	//	{	
	//		// Clear the custom timer....
	//		if (StatComponent->GetCurrentGunWeapon())
	//		{
	//			AWSniperRifle* Sniper = Cast<AWSniperRifle>(StatComponent->GetCurrentGunWeapon());
	//			if (Sniper)
	//			{
	//				Sniper->SetSniperScopeState(EScope::NOSCOPE);
	//				Sniper->CancelScope();
	//				Sniper->CancelTimer();
	//			}
	//			StatComponent->GetCurrentGunWeapon()->ResetTimeHandle();
	//		}
	//		else
	//		{
	//			StatComponent->GetCurrentWeapon()->ResetTimeHandle();
	//		}

	//		FPSmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
	//		Thirdmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
	//		
	//		PutNewWeapon(Weapon);
	//	}
	//}

	ServerChangeWeapon(this, ChangeWeaponNumber);

}

void AFPSCharacter::ChangeViewCamera(bool Flag)
{
	FirstView = Flag;


	if (FirstView)
	{
		//GetCapsuleComponent()->bHiddenInGame = false;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		SpringArmComponent->TargetArmLength = 0.f;
		SpringArmComponent->bUsePawnControlRotation = false;
		SpringArmComponent->bInheritPitch = false;
		SpringArmComponent->bInheritRoll = false;
		SpringArmComponent->bInheritYaw = false;
		FPSCameraComponent->SetRelativeLocation(FVector(0, 0, 65));
		GetMesh()->SetOwnerNoSee(true);

		for (int i = 0; i < FPSmesh.Num(); ++i)
		{
			FPSmesh[i]->SetOwnerNoSee(false);
			//FPSmesh[i]->SetOnlyOwnerSee(true);
		}

		for (int i = 0; i < Thirdmesh.Num(); ++i)
		{
			Thirdmesh[i]->SetOwnerNoSee(true);
		}
	}

	else
	{
		//GetCapsuleComponent()->bHiddenInGame = false;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		SpringArmComponent->TargetArmLength = 300;
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritRoll = true;
		SpringArmComponent->bInheritYaw = true;
		GetMesh()->SetOwnerNoSee(false);


		for (int i = 0; i < FPSmesh.Num(); ++i)
		{
			FPSmesh[i]->SetOwnerNoSee(true);
		}

		for (int i = 0; i < Thirdmesh.Num(); ++i)
		{
			Thirdmesh[i]->SetOwnerNoSee(false);
		}
	}
}

void AFPSCharacter::StartJump()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (!GetMovementComponent()->IsFalling())
	{
		IsJumpHeld = true;
		StatComponent->SetCharacterLowerState(LOWER_STATE::JUMP);
	}
}

void AFPSCharacter::StopJump()
{
	IsJumpHeld = false;
}

void AFPSCharacter::ChangeViewPort(bool ScopeOn)
{
	if (FPSUIWidget)
	{
		if (ScopeOn)
		{
			FPSUIWidget->GetScope()->SetRenderOpacity(1.f);
			FPSUIWidget->GetCrosshair()->SetRenderOpacity(0.f);
			//FPSUIWidget->GetFPSRender()->SetRenderOpacity(0.f);
		}

		else
		{
			FPSUIWidget->GetScope()->SetRenderOpacity(0.f);
			FPSUIWidget->GetCrosshair()->SetRenderOpacity(1.f);
			//FPSUIWidget->GetFPSRender()->SetRenderOpacity(1.f);
		}
	}
}

FName AFPSCharacter::GetCurrentWeaponNumberName()
{
	if (!StatComponent) return TEXT("NONE");

	switch (StatComponent->GetCurrentGunWeapon()->eGunNumber)
	{
	case EGunNumber::AR_AK:
		return TEXT("AR_AK");
	case EGunNumber::CARBINE:
		return TEXT("CARBINE");
	case EGunNumber::MG_M249:
		return TEXT("MG_M249");
	case EGunNumber::ONEHANDED:
		return TEXT("ONEHANDED");
	case EGunNumber::RIFLE:
		return TEXT("RIFLE");
	case EGunNumber::SHOTGUN:
		return TEXT("SHOTGUN");
	case EGunNumber::SMG_MP5:
		return TEXT("SMG_MP5");
	case EGunNumber::TWOHANDED:
		return TEXT("TWOHANDED");

	default:
		return TEXT("NONE");
	}
}

float AFPSCharacter::GetRelevantAnimTime(FName MachineState, FName StateName)
{
	if (!Instance)
	{
		Instance = GetMesh()->GetAnimInstance();
	}


	if (Instance)
	{
		FAnimNode_StateMachine* machine = Instance->GetStateMachineInstanceFromName(MachineState);
		if (machine)
		{
			IAnimClassInterface* iface = IAnimClassInterface::GetFromClass(animInstance->GetClass());
			const FBakedAnimationStateMachine* baked = Instance->GetMachineDescription(iface, machine);

			if (baked)
			{
				int stateIdx = baked->FindStateIndex(StateName);
				return animInstance->GetRelevantAnimTime(Instance->GetStateMachineIndex(MachineState), stateIdx);
			}
		}
	}
	return 0.0f;
}

void AFPSCharacter::SmoothingCrouch(float DeltaTime)
{
	if (GetMovementComponent()->IsFalling())
	{
		return;
	}

	if (StatComponent && !StatComponent->GetIsDead())
	{
		const float TargetBEH = IsCrouchHeld ? CrouchedEyeHeight : DecBEH;

		if (Controller != NULL)
		{
			if (FirstView)
			{
				FPSCameraComponent->SetRelativeLocation(FMath::VInterpTo(FPSCameraComponent->GetRelativeLocation(),
					FVector(FPSCameraComponent->GetRelativeLocation().X,
						FPSCameraComponent->GetRelativeLocation().Y,
						TargetBEH), DeltaTime, 8.f));
			}
		}
	}

}


void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// float..
	DOREPLIFETIME(AFPSCharacter, MoveForwardValue);
	DOREPLIFETIME(AFPSCharacter, MoveRightValue);
	DOREPLIFETIME(AFPSCharacter, AimOffsetPitch);
	DOREPLIFETIME(AFPSCharacter, AimOffsetYaw);
	DOREPLIFETIME(AFPSCharacter, IsWalkHeld);
	DOREPLIFETIME(AFPSCharacter, IsCrouchHeld);
	DOREPLIFETIME(AFPSCharacter, AttackAnimCall);
	DOREPLIFETIME(AFPSCharacter, bIsReloading);
	DOREPLIFETIME(AFPSCharacter, ReloadStartTime);
	DOREPLIFETIME(AFPSCharacter, CurrentAnimationWeaponNumber);
	DOREPLIFETIME(AFPSCharacter, DelayTime);
}

void AFPSCharacter::StartWalk()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (!IsCrouchHeld) {
		IsWalkHeld = true;
		GetCharacterMovement()->MaxWalkSpeed = StatComponent->WalkSpeed;

		if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
		{
			SyncClientSpeed(GetCharacterMovement()->MaxWalkSpeed);
			SyncClientWalk(IsWalkHeld);
		}
	}
}

void AFPSCharacter::StopWalk()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (IsWalkHeld)
	{
		IsWalkHeld = false;
		GetCharacterMovement()->MaxWalkSpeed = StatComponent->GetCurrentWeapon()->GetRunSpeed();

		if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
		{
			SyncClientSpeed(GetCharacterMovement()->MaxWalkSpeed);
			SyncClientWalk(IsWalkHeld);
		}
	}
}

void AFPSCharacter::SyncClientWalk_Implementation(bool flag)
{
	IsWalkHeld = flag;
}

void AFPSCharacter::StartCrouch()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (!GetMovementComponent()->IsFalling() && !GetMovementComponent()->IsCrouching())
	{
		IsCrouchHeld = true;
		StatComponent->SetCharacterLowerState(LOWER_STATE::CROUCH);

		GetCharacterMovement()->bWantsToCrouch = true;

		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::RELOAD)
		{
			ReloadStartTime = GetRelevantAnimTime(GetCurrentWeaponNumberName(), TEXT("Reload"));
			//UE_LOG(LogTemp, Warning, TEXT("%.1f"), ReloadStartTime);
		}

		CrouchMeshPos();
	}
}

void AFPSCharacter::StopCrouch()
{
	if (!StatComponent) return;

	else
	{
		if (StatComponent->GetIsDead())
		{
			return;
		}
	}

	if (IsCrouchHeld)
	{
		IsCrouchHeld = false;
		//GetCharacterMovement()->MaxWalkSpeed = StatComponent->GetCurrentWeapon()->GetRunSpeed();
		GetCharacterMovement()->bWantsToCrouch = false;

		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::RELOAD)
		{
			ReloadStartTime = GetRelevantAnimTime(GetCurrentWeaponNumberName(), TEXT("Crouch_Reload"));
			//UE_LOG(LogTemp, Warning, TEXT("%.1f"), ReloadStartTime);
		}

		CrouchMeshPos();
	}
}

void AFPSCharacter::CrouchMeshPos()
{
	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
	{
		SyncClientSpeed(GetCharacterMovement()->MaxWalkSpeed);
		SyncClientCrouch(IsCrouchHeld, ReloadStartTime);
	}
}


void AFPSCharacter::SyncClientCrouch_Implementation(bool flag, float Time)
{
	IsCrouchHeld = flag;
	ReloadStartTime = Time;

	if (IsCrouchHeld)
	{
		GetCharacterMovement()->bWantsToCrouch = true;
	}

	else
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}

}

UFPSHUDWidget* AFPSCharacter::GetFPSUIWidget()
{
	if (!FPSUIWidget)
	{
		if (HUDWidgetClass)
		{
			HUDWidget = CreateWidget(GetPlayerController(), HUDWidgetClass);
		}

		FPSUIWidget = Cast<UFPSHUDWidget>(HUDWidget);
	}

	return FPSUIWidget;
}

void AFPSCharacter::SetUpperState(UPPER_STATE ChangeState)
{
	if (!StatComponent) return;

	StatComponent->SetCharacterUpperState(ChangeState);
}

bool AFPSCharacter::SetWeaponMethod(AWBase* Weapon, bool Dropping)
{
	if (!Weapon) return false;


	if (Dropping)
	{
		if (StatComponent->GetSelectWeapon(Weapon->eWeaponNum))
		{
			if (Weapon->eWeaponNum == EWeaponNum::E_Rifle || Weapon->eWeaponNum == EWeaponNum::E_Sub)
			{
				DropWeapon(Weapon->eWeaponNum);
			}

			else
			{
				return false;
			}
		}
	}

	SetNewWeaponMesh(Weapon);
	if (!StatComponent->GetCurrentWeapon())
	{
		PutNewWeapon(Weapon);
		return true;
	}

	return false;
}

bool AFPSCharacter::PurchaseWeapon(ECreatWeaponNum WeaponNumber)
{
	TSubclassOf<AWBase> SpawnWeapon;

	if (SpawnWeapon = GetWeaponBP(WeaponNumber))
	{
		AWBase* EqualWeapon = SpawnWeapon->GetDefaultObject<AWBase>();
		//SyncClientSpawnWeapon(this, SpawnWeapon, FVector(0, 0, 5000));

		TArray<AActor*> ActorArray;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWBase::StaticClass(), ActorArray);

		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("%s"), *EqualWeapon->GetSocketName().ToString()));

		for (int i = 0; i < ActorArray.Num(); ++i)
		{
			if (AWBase* Weapon = Cast<AWBase>(ActorArray[i]))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("%s"), *Weapon->GetSocketName().ToString()));
				if (Weapon->GetSocketName().IsEqual(EqualWeapon->GetSocketName()))
				{
					SetWeaponMethod(Weapon, false);
					return true;
				}
			}
		}

		//TArray<FHitResult> Hit;
		//bool bSucess = GetWorld()->LineTraceMultiByChannel(Hit,
		//	FVector(0, 0, 6000), FVector(0, 0, 6000) - FVector(0, 0, 4500),
		//	ECollisionChannel::ECC_Visibility);

		//if (bSucess)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("%d"), Hit.Num()));
		//	for (int i = 0; i < Hit.Num(); ++i)
		//	{
		//		if (Hit[i].GetActor())
		//		{
		//			if (AWBase* Weapon = Cast<AWBase>(Hit[i].GetActor()))
		//			{
		//				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("%s"), *Weapon->GetSocketName().ToString()));
		//				if (Weapon->GetSocketName().IsEqual(EqualWeapon->GetSocketName()))
		//				{
		//					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Get Purchased Weapon!!!"));
		//					SetWeaponMethod(Weapon, true);
		//					return true;
		//				}
		//			}
		//		}
		//	}
		//}


		return false;
	}

	return false;
}

void AFPSCharacter::SetNewWeaponMesh(AWBase* WeaponActor)
{
	//if (!GetFPSCharacterStatComponent()) return;

	//uint8 num = uint8(WeaponActor->eWeaponNum);

	//if (GetFPSCharacterStatComponent()->GetSelectWeapon(WeaponActor->eWeaponNum))
	//{
	//	GetFPSCharacterStatComponent()->RemoveSelectWeapon(WeaponActor->eWeaponNum);
	//}

	//GetFPSCharacterStatComponent()->SetArrayWeapon(WeaponActor);
	//Thirdmesh[num - 1]->SetSkeletalMesh(WeaponActor->TPSWeaponComponent->SkeletalMesh);
	//Thirdmesh[num - 1]->AttachToComponent(GetMesh(),
	//	FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponActor->GetSocketName());

	//if (WeaponActor->eWeaponNum == EWeaponNum::E_Rifle)
	//{
	//	RestBackWeapon();
	//}

	//else
	//{
	//	Thirdmesh[num - 1]->SetVisibility(false);
	//}

	//FPSmesh[num - 1]->SetSkeletalMesh(WeaponActor->FPSHandComponent->SkeletalMesh);
	//FPSmesh[num - 1]->SetCastInsetShadow(false);
	//FPSmesh[num - 1]->SetCastShadow(false);
	//FPSmesh[num - 1]->SetVisibility(false);


	//WeaponActor->WeaponComponent->SetSimulatePhysics(false);
	//WeaponActor->WeaponComponent->SetCollisionProfileName(TEXT("PickedWeapon"));
	//WeaponActor->SetActorHiddenInGame(true);
	//WeaponActor->SetPossiblePickWeapon(false);


	ServerEquipWeapon(this, WeaponActor);

}

void AFPSCharacter::PutNewWeapon(AWBase* WeaponActor)
{
	/*if (!GetFPSCharacterStatComponent()) return;

	GetFPSCharacterStatComponent()->SetCharacterCurrentWeapon(WeaponActor);

	if (!WeaponActor)
	{
		CurrentAnimationWeaponNumber = 0;
		ServerTakeoutWeapon(0);

		return;
	}

	switch (WeaponActor->eWeaponNum)
	{
	case EWeaponNum::E_Rifle:
		GetFPSCharacterStatComponent()->SetCharacterCurrentGun(Cast<AWGun>(WeaponActor));
		Thirdmesh[uint8(WeaponActor->eWeaponNum) - 1]->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponActor->GetSocketName());
		break;
	case EWeaponNum::E_Sub:
		GetFPSCharacterStatComponent()->SetCharacterCurrentGun(Cast<AWGun>(WeaponActor));
		break;
	case EWeaponNum::E_Knife:
	case EWeaponNum::E_C4:
	case EWeaponNum::E_Grenade:
	case EWeaponNum::E_Flash:
	case EWeaponNum::E_Smoke:
		GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
		break;
	}

	if (WeaponActor->eWeaponNum != EWeaponNum::E_Rifle && GetFPSCharacterStatComponent()->GetSelectWeapon(EWeaponNum::E_Rifle))
	{
		RestBackWeapon();
	}

	if (GetFPSCharacterStatComponent()->GetCurrentGunWeapon())
	{
		GetFPSCharacterStatComponent()->GetCurrentGunWeapon()->SetCamShake(FPSCameraAttack);
	}

	GetFPSCharacterStatComponent()->SetCharacterCurrentWeaponNum();
	Thirdmesh[uint8(WeaponActor->eWeaponNum) - 1]->SetVisibility(true);

	if (GetPlayerController())
	{
		if (GetPlayerController()->IsLocalController())
		{
			if (GetFPSUIWidget())
			{
				FPSmesh[uint8(GetFPSCharacterStatComponent()->GetCurrentWeaponNumber()) - 1]->SetVisibility(true);
				GetFPSUIWidget()->SetAmmoCount(this);
				GetFPSCharacterStatComponent()->GetCurrentWeapon()->Draw(this);
			}
		}
	}

	CurrentAnimationWeaponNumber = uint8(WeaponActor->eGunNumber);*/

	ServerTakeoutWeapon(this, WeaponActor);
}

void AFPSCharacter::DropWeapon(EWeaponNum WeaponNum)
{
	//if (!GetFPSCharacterStatComponent()) return;

	//AWBase* Weapon;

	//switch (WeaponNum)
	//{
	//case EWeaponNum::E_Grenade:
	//case EWeaponNum::E_Flash:
	//case EWeaponNum::E_Smoke:
	//case EWeaponNum::E_Knife:
	//	return;

	//case EWeaponNum::E_Sub:
	//case EWeaponNum::E_Rifle:
	//case EWeaponNum::E_C4:
	//	Weapon = GetFPSCharacterStatComponent()->GetSelectWeapon(WeaponNum);
	//	break;

	//default:
	//	return;
	//}

	//if (Weapon)
	//{
	//	if (StatComponent->GetCurrentWeapon() && StatComponent->GetCurrentWeapon() == Weapon)
	//	{
	//		FPSmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
	//		Thirdmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);

	//		GetFPSCharacterStatComponent()->RemoveSelectWeapon(StatComponent->GetCurrentWeapon()->eWeaponNum);
	//		GetFPSCharacterStatComponent()->SetCharacterCurrentWeapon(nullptr);

	//		if (Weapon->eWeaponNum == EWeaponNum::E_Rifle || Weapon->eWeaponNum == EWeaponNum::E_Sub)
	//		{
	//			GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
	//		}
	//	}

	//	FVector Location;
	//	FRotator Rotation;
	//	GetController()->GetPlayerViewPoint(Location, Rotation);
	//	Location += FPSCameraComponent->GetForwardVector() * 130.f;
	//	EWeaponNum Num = DropAndEnableChangeWeapon();

	//	Weapon->SetOwningPlayer(nullptr);
	//	Weapon->SetActorHiddenInGame(false);
	//	Weapon->WeaponComponent->SetSimulatePhysics(true);
	//	Weapon->WeaponComponent->SetCollisionProfileName(TEXT("Weapon"));
	//	Weapon->SetActorLocation(Location);
	//	Weapon->WeaponComponent->AddImpulse(FPSCameraComponent->GetForwardVector() *
	//		500.f * Weapon->WeaponComponent->GetMass() + FPSCameraComponent->GetUpVector() * 100.f);
	//	Weapon->SetPossiblePickWeapon(true);

	//}

	//Location..

	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);
	Location += FPSCameraComponent->GetForwardVector() * 130.f;

	//Impulse...
	//FPSCameraComponent->GetForwardVector() *
	//	500.f * Weapon->WeaponComponent->GetMass() + FPSCameraComponent->GetUpVector() * 100.f;


	AWBase* Weapon = GetFPSCharacterStatComponent()->GetSelectWeapon(WeaponNum);

	if (Weapon)
	{
		if (AWGun* Gun = Cast<AWGun>(Weapon))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%.1f, %.1f"), Gun->GetCurrentAmmoCount(), Gun->GetElseAmmoCount()));
			ServerDropedWeapon(this, WeaponNum, Location, FPSCameraComponent->GetForwardVector() *
				200000.f + FPSCameraComponent->GetUpVector() * 100.f, Gun->GetCurrentAmmoCount(), Gun->GetElseAmmoCount());
		}

		else
		{
			ServerDropedWeapon(this, WeaponNum, Location, FPSCameraComponent->GetForwardVector() *
				200000.f + FPSCameraComponent->GetUpVector() * 100.f, 0, 0);
		}
	}




}


EWeaponNum AFPSCharacter::DropAndEnableChangeWeapon()
{

	if (!StatComponent) return EWeaponNum::E_None;

	switch (StatComponent->GetCurrentWeaponNumber())
	{
	case EWeaponNum::E_Flash:
		return EWeaponNum::E_None;

	case EWeaponNum::E_Grenade:
		return EWeaponNum::E_None;

	case EWeaponNum::E_Smoke:
		return EWeaponNum::E_None;

	case EWeaponNum::E_Knife:
		return EWeaponNum::E_None;

	case EWeaponNum::E_Rifle:
		//GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Sub))
		{
			ChangeWeapon(EWeaponNum::E_Sub);
		}
		else if(StatComponent->GetSelectWeapon(EWeaponNum::E_Knife))
		{
			ChangeWeapon(EWeaponNum::E_Knife);
		}
		else
		{
			ChangeWeapon(EWeaponNum::E_None);
		}

		return EWeaponNum::E_Rifle;
	case EWeaponNum::E_Sub:
		//GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Rifle))
		{
			ChangeWeapon(EWeaponNum::E_Rifle);
		}
		else if(StatComponent->GetSelectWeapon(EWeaponNum::E_Knife))
		{
			ChangeWeapon(EWeaponNum::E_Knife);
		}
		else
		{
			ChangeWeapon(EWeaponNum::E_None);
		}
		return EWeaponNum::E_Sub;

	case EWeaponNum::E_C4:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Rifle))
		{
			ChangeWeapon(EWeaponNum::E_Rifle);
		}
		else if(StatComponent->GetSelectWeapon(EWeaponNum::E_Sub))
		{
			ChangeWeapon(EWeaponNum::E_Sub);
		}
		else if(StatComponent->GetSelectWeapon(EWeaponNum::E_Knife))
		{
			ChangeWeapon(EWeaponNum::E_Knife);
		}

		else
		{
			ChangeWeapon(EWeaponNum::E_None);
		}

		return EWeaponNum::E_C4;

	default:
		ChangeWeapon(EWeaponNum::E_None);
		break;
	}


	return EWeaponNum::E_None;
}

FString AFPSCharacter::GetStateAsString(UPPER_STATE Value)
{
	switch (Value)
	{
	case UPPER_STATE::ACTION:
		return TEXT("Action");
	case UPPER_STATE::ATTACK:
		return TEXT("Attack");
	case UPPER_STATE::DRAW:
		return TEXT("Draw");
	case UPPER_STATE::IDLE:
		return TEXT("Idle");
	case UPPER_STATE::RELOAD:
		return TEXT("Reload");
	default:
		return TEXT("Invalid");
	}
}

FString AFPSCharacter::GetStateAsString(LOWER_STATE Value)
{
	switch (Value)
	{
	case LOWER_STATE::CROUCH:
		return TEXT("Crouch");
		break;
	case LOWER_STATE::JUMP:
		return TEXT("Jump");
		break;
	case LOWER_STATE::IDLE:
		return TEXT("Idle");
		break;
	default:
		return TEXT("Invalid");
		break;
	}
}

void AFPSCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	TakeWeapon(OtherActor);
}

void AFPSCharacter::TakeWeapon(FHitResult Hit)
{
	if (Hit.GetActor() && Hit.Actor->IsA(AWBase::StaticClass()))
	{
		if (AWBase* HitWeapon = Cast<class AWBase>(Hit.Actor))
		{
			if (HitWeapon->GetPossiblePickWeapon())
			{
				SetWeaponMethod(HitWeapon, true);
			}
		}
	}
}

void AFPSCharacter::TakeWeapon(AActor* Actor)
{
	if (Actor->IsA(AWBase::StaticClass()))
	{
		if (AWBase* Weapon = Cast<class AWBase>(Actor))
		{
			if (Weapon->GetPossiblePickWeapon())
			{
				if (!StatComponent->GetSelectWeapon(Weapon->eWeaponNum))
				{
					SetWeaponMethod(Weapon, false);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("don't Add weapon"));
				}
			}
		}
	}
}

USkeletalMeshComponent* AFPSCharacter::GetCurrentFPSMesh()
{
	if (StatComponent)
	{
		if (FPSmesh.Num() <= 0)
		{
			return nullptr;
		}

		if (StatComponent->GetCurrentWeapon())
		{
			return FPSmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1];
		}

	}

	return nullptr;
}

USkeletalMeshComponent* AFPSCharacter::GetCurrentThirdMesh()
{
	if (StatComponent)
	{
		if (Thirdmesh.Num() <= 0)
		{
			return nullptr;
		}

		if (StatComponent->GetCurrentWeapon())
		{
			return Thirdmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1];
		}
	}

	return nullptr;
}

void AFPSCharacter::RestBackWeapon() 
{
	if (!StatComponent)
	{
		return;
	}

	AWBase* Weapon = FindWeapon(EWeaponNum::E_Rifle);
	if (Weapon)
	{
		AWGun* Gun = Cast<AWGun>(Weapon);
		if (Gun)
		{
			Thirdmesh[uint8(EWeaponNum::E_Rifle) - 1]->SetVisibility(true);
			Thirdmesh[uint8(EWeaponNum::E_Rifle) - 1]->AttachToComponent(GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale, Gun->GetBackSocketName());
		}
	}
}

AActorPool* AFPSCharacter::GetActorPool()
{
	return Cast<AActorPool>(UGameplayStatics::GetActorOfClass(GetWorld(), AActorPool::StaticClass()));
}

EBoneHit AFPSCharacter::CheckHit(FString HitBoneName)
{
	if (HitBoneName.Equals(TEXT("Bip01-Head")))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,TEXT("Head Shot!"));
		return EBoneHit::EB_HEAD;
	}

	else if (HitBoneName.Equals(TEXT("Bip01-Spine1")))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Shot!"));
		return EBoneHit::EB_NONE;
	}

	else if (HitBoneName.Equals(TEXT("Bip01-Pelvis")))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Guts Shot!"));
		return EBoneHit::EB_GUTS;
	}

	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Leg or Arm Shot!"));
		return EBoneHit::EB_LEG;
	}
}

void AFPSCharacter::MoveForward(float Value)
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	if ((Controller != NULL) && (Value != 0.0f))
	{
		rotating = true;
		// 어느 쪽이 전방인지 알아내어, 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
		FRotator Val = Controller->GetControlRotation();
		Val.Pitch = 0;
		Val.Roll = 0;

		FVector Direction = FRotationMatrix(Val).GetUnitAxis(EAxis::X);
		Direction.Normalize();

		AddMovementInput(Direction, Value);
	}

	if (GetLocalRole() < ROLE_Authority && MoveForwardValue != Value && IsLocallyControlled())
	{
		SyncClientForward(Value);
	}

	MoveForwardValue = Value;
}


void AFPSCharacter::SyncClientForward_Implementation(float Value)
{
	MoveForwardValue = Value;
	AimOffsetPitch = 0.f;
	AimOffsetYaw = 0.f;
}


void AFPSCharacter::MoveRight(float Value)
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (FMath::IsNearlyZero(GetInputAxisValue("MoveForward")))
		{
			if (GetLocalRole() < ROLE_Authority && MoveForwardValue != 1.f)
			{
				SyncClientForward(1.f);
			}

			MoveForwardValue = 1.f;
		}

		rotating = true;
		// 어느 쪽이 오른쪽인지 알아내어, 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
		FRotator Val = Controller->GetControlRotation();
		Val.Pitch = 0;
		Val.Roll = 0;

		FVector Direction = FRotationMatrix(Val).GetUnitAxis(EAxis::Y);
		//Direction.Z = .0f;
		Direction.Normalize();

		AddMovementInput(Direction, Value);
	}

	if (GetLocalRole() < ROLE_Authority && MoveRightValue != Value && IsLocallyControlled())
	{
		SyncClientRight(Value);
	}

	MoveRightValue = Value;
}

void AFPSCharacter::SyncClientRight_Implementation(float RightValue)
{
	MoveRightValue = RightValue;
}

void AFPSCharacter::AddControllerYawInput(float Val)
{

	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	Val = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	float mulvalue = Val * Sensitive;

	Super::AddControllerYawInput(mulvalue);

	if (CaculatingDot(GetActorForwardVector(), FPSCameraComponent->GetForwardVector()) <= 0.f)
	{
		rotating = true;
	}

	if (Val != 0)
	{
		RotatingAimOffset(GetActorRotation(), GetControlRotation());
	}

	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
	{
		SyncClientRotation(AimOffsetYaw, AimOffsetPitch);
	}
}

void AFPSCharacter::AddControllerPitchInput(float Val)
{
	if (StatComponent && StatComponent->GetIsDead())
	{
		return;
	}

	Val = Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

	float mulvalue = Sensitive * Val;

	Super::AddControllerPitchInput(mulvalue);

	if (Val != 0)
	{
		RotatingAimOffset(GetActorRotation(), GetControlRotation());
	}


	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
	{
		SyncClientRotation(AimOffsetYaw, AimOffsetPitch);
	}

}

// Aim offset..
void AFPSCharacter::RotatingAimOffset(FRotator Actor, FRotator Control)
{
	FRotator offsetResult = Actor - Control;

	AimOffsetYaw = FMath::ClampAngle(offsetResult.Yaw, -90, 90);
	AimOffsetPitch = FMath::ClampAngle(offsetResult.Pitch, -90, 90);
} 

// While Moving.. Rotating Lower Hip Replication..
void AFPSCharacter::RotatingLowerHips(float DeltaTime)
{
	if (!FMath::IsNearlyZero(MoveRightValue) && FMath::IsNearlyZero(GetInputAxisValue("MoveForward")))
	{
		LowerHipsRotation = MoveRightValue * 60.f;
	}

	else
	{
		LowerHipsRotation = MoveRightValue * 35.f;
		LowerHipsRotation *= MoveForwardValue;
	}

	CurrentLowerHipsRotation = FMath::FInterpTo(CurrentLowerHipsRotation, LowerHipsRotation, DeltaTime, 10);

}
// .........
void AFPSCharacter::ResetLowerHips(float DeltaTime)
{
	if (!rotating)
		return;

	if (GetController()->GetControlRotation().Equals(FRotator(GetController()->GetControlRotation().Pitch, 
		GetController()->GetPawn()->GetActorRotation().Yaw, GetController()->GetControlRotation().Roll), 0.1f))
	{
		rotating = false;
	}

	GetController()->GetPawn()->SetActorRotation(
		FMath::RInterpTo(GetController()->GetPawn()->GetActorRotation(),
			FRotator(0, GetController()->GetControlRotation().Yaw, 0),
			DeltaTime, 20));

	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled())
	{
		SyncClientActorRotation(GetActorRotation());
	}
}

void AFPSCharacter::SyncClientSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AFPSCharacter::SyncClientReload_Implementation(bool flag)
{
	bIsReloading = flag;
}


void AFPSCharacter::SyncClientRotation_Implementation(float Yaw, float Pitch)
{
	//SetActorRotation(ActorRotation);
	//RotatingAimOffset(ActorRotation, ControllerRotation);
	//CurrentLowerHipsRotation = Rotation;

	AimOffsetPitch = Pitch;
	AimOffsetYaw = Yaw;
}

void AFPSCharacter::SyncClientActorRotation_Implementation(FRotator ActorRotation)
{
	SetActorRotation(ActorRotation);
}

void AFPSCharacter::SyncClientSpawnEffect_Implementation(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End)
{
	ServerSpawnEffect(ParticleEffect, MeshComp, End);
}

void AFPSCharacter::ServerSpawnEffect_Implementation(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End)
{
	//UGameplayStatics::SpawnEmitterAttached(MuzzleParticleEffect, MeshComp, TEXT("Muzzle"));
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticleEffect, MeshComp->GetSocketLocation(TEXT("Muzzle")));

	if (!IsLocallyControlled())
	{
		UNiagaraComponent* Particle =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleEffect, MeshComp->GetSocketLocation(TEXT("Muzzle")));

		if (Particle)
		{
			Particle->SetVectorParameter("BeamEnd", End /*- MeshComp->GetSocketLocation(TEXT("Muzzle"))*/);
		}
	}
}

void AFPSCharacter::SyncSpawnPenetrateEffect_Implementation(UNiagaraSystem* ParticleEffect, FVector Start, FVector End)
{
	ServerSpawnPenetrateEffect(ParticleEffect, Start, End);
}

void AFPSCharacter::ServerSpawnPenetrateEffect_Implementation(UNiagaraSystem* ParticleEffect, FVector Start, FVector End)
{
	if (!IsLocallyControlled())
	{
		UNiagaraComponent* Particle =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleEffect, Start);

		if (Particle)
		{
			Particle->SetVectorParameter("BeamEnd", End);
		}
	}
}

void AFPSCharacter::SyncClientSpawnMuzzleEffect_Implementation(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName)
{
	ServerSpawnMuzzleEffect(MuzzleEffect, MeshComp, SocketName);
}

void AFPSCharacter::ServerSpawnMuzzleEffect_Implementation(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName)
{
	if (!IsLocallyControlled())
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, SocketName);
	}
}

void AFPSCharacter::SyncClientSpawnDecal_Implementation(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot)
{
	ServerSpawnDecal(DecalBluePrint, Pos, Rot);
}

void AFPSCharacter::ServerSpawnDecal_Implementation(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot)
{
	ADecalActor* Decal = GetWorld()->SpawnActor<ADecalActor>(DecalBluePrint);

	if (Decal)
	{
		Decal->SetActorLocationAndRotation(Pos, Rot);
		Decal->SetActorHiddenInGame(false);
	}
}


void AFPSCharacter::SyncClientSpawnShell_Implementation(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp)
{
	ServerSpawnShell(ShellBlueprint, Impulse, MeshComp);
}

void AFPSCharacter::ServerSpawnShell_Implementation(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp)
{
	if (!IsLocallyControlled())
	{
		AStaticMeshActor* Shell = nullptr;
		Shell = GetWorld()->SpawnActor<AStaticMeshActor>(ShellBlueprint);

		if (Shell)
		{
			Shell->SetActorLocation(MeshComp->GetSocketLocation(TEXT("Shell")));
			Shell->GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			Shell->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
			Shell->SetActorHiddenInGame(false);
			Shell->GetStaticMeshComponent()->AddImpulse(Impulse);
		}
	}
}

void AFPSCharacter::SyncClientSpawnHitEffect_Implementation(UParticleSystem* BloodParticle, FVector Hit)
{
	ServerSpawnHitEffect(BloodParticle, Hit);
}

void AFPSCharacter::ServerSpawnHitEffect_Implementation(UParticleSystem* BloodParticle, FVector Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, FTransform(Hit), true);
}

void AFPSCharacter::SyncClientSpawnSound_Implementation(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp)
{
	ServerSpawnSound(ShotSound, MeshComp);
}

void AFPSCharacter::ServerSpawnSound_Implementation(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp)
{
	/*if (!HasAuthority() && !IsLocallyControlled())
	{
		UGameplayStatics::SpawnSoundAttached(ShotSound, MeshComp, TEXT("Muzzle"));
	}*/

	UGameplayStatics::SpawnSoundAttached(ShotSound, MeshComp, TEXT("Muzzle"));
}

void AFPSCharacter::SyncClientSpawnWeapon_Implementation(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location)
{
	//AWBase* OutWeapon = GetWorld()->SpawnActor<AWBase>(WeaponBP, FTransform(Location));
	//FHitResult Hit;

	//if (OutWeapon)
	//{
	//	OutWeapon->WeaponComponent->SetSimulatePhysics(false);

	//	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit,
	//		FVector(0, 0, 5500), FVector(0, 0, 5500) - Location,
	//		ECollisionChannel::ECC_Visibility);

	//	if (bSucess)
	//	{
	//		if (Hit.GetActor())
	//		{
	//			if (AWBase* Weapon = Cast<AWBase>(Hit.GetActor()))
	//			{
	//				//MulticastGetSpawnWeapon(Character, OutWeapon);
	//				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Get Purchased Weapon!!!"));
	//			}
	//		}
	//		
	//	}
	//}

	MulticastSpawnWeapon(Character, WeaponBP, Location);
}

void AFPSCharacter::MulticastSpawnWeapon_Implementation(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location)
{
	AWBase* OutWeapon = GetWorld()->SpawnActor<AWBase>(WeaponBP, FTransform(Location));

	if (OutWeapon)
	{
		OutWeapon->WeaponComponent->SetSimulatePhysics(false);
	}
}

void AFPSCharacter::ServerEquipWeapon_Implementation(AFPSCharacter* Character, AWBase* PickedWeapon)
{
	ClientEquipWeapon(Character, PickedWeapon);
}

void AFPSCharacter::ClientEquipWeapon_Implementation(AFPSCharacter* Character, AWBase* PickedWeapon)
{
	if (!Character->GetFPSCharacterStatComponent()) return;

	uint8 num = uint8(PickedWeapon->eWeaponNum);

	if (Character->GetFPSCharacterStatComponent()->GetSelectWeapon(PickedWeapon->eWeaponNum))
	{
		Character->GetFPSCharacterStatComponent()->RemoveSelectWeapon(PickedWeapon->eWeaponNum);
	}
	Character->GetFPSCharacterStatComponent()->SetArrayWeapon(PickedWeapon);


	//Character->Thirdmesh[num - 1]->AttachToComponent(Character->GetMesh(),
	//	FAttachmentTransformRules::SnapToTargetNotIncludingScale, PickedWeapon->GetSocketName());

	if (PickedWeapon->eWeaponNum == EWeaponNum::E_Rifle)
	{
		Character->RestBackWeapon();
	}

	else
	{
		Character->Thirdmesh[num - 1]->SetVisibility(false);
	}

	Character->Thirdmesh[num - 1]->SetSkeletalMesh(PickedWeapon->TPSWeaponComponent->SkeletalMesh);
	Character->FPSmesh[num - 1]->SetSkeletalMesh(PickedWeapon->FPSHandComponent->SkeletalMesh);
	Character->FPSmesh[num - 1]->SetCastInsetShadow(false);
	Character->FPSmesh[num - 1]->SetCastShadow(false);
	Character->FPSmesh[num - 1]->SetVisibility(false);


	PickedWeapon->WeaponComponent->SetSimulatePhysics(false);
	PickedWeapon->WeaponComponent->SetCollisionProfileName(TEXT("PickedWeapon"));
	PickedWeapon->SetActorHiddenInGame(true);
	PickedWeapon->SetPossiblePickWeapon(false);
}

void AFPSCharacter::ServerTakeoutWeapon_Implementation(AFPSCharacter* Character, AWBase* Weapon)
{
	//CurrentAnimationWeaponNumber = ChangeNumber;
	ClientTakeoutWeapon(Character, Weapon);
}

void AFPSCharacter::ClientTakeoutWeapon_Implementation(AFPSCharacter* Character, AWBase* Weapon)
{
	if (!Character->GetFPSCharacterStatComponent()) return;

	Character->GetFPSCharacterStatComponent()->SetCharacterCurrentWeapon(Weapon);

	if (!Weapon)
	{
		Character->GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
		Character->CurrentAnimationWeaponNumber = uint8(0);
	}

	else
	{
		switch (Weapon->eWeaponNum)
		{
		case EWeaponNum::E_Rifle:
			Character->GetFPSCharacterStatComponent()->SetCharacterCurrentGun(Cast<AWGun>(Weapon));
			break;
		case EWeaponNum::E_Sub:
			Character->GetFPSCharacterStatComponent()->SetCharacterCurrentGun(Cast<AWGun>(Weapon));
			break;
		case EWeaponNum::E_Knife:
		case EWeaponNum::E_C4:
		case EWeaponNum::E_Grenade:
		case EWeaponNum::E_Flash:
		case EWeaponNum::E_Smoke:
			Character->GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
			break;
		}

		Character->Thirdmesh[uint8(Weapon->eWeaponNum) - 1]->AttachToComponent(Character->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetSocketName());

		if (Weapon->eWeaponNum != EWeaponNum::E_Rifle && Character->GetFPSCharacterStatComponent()->GetSelectWeapon(EWeaponNum::E_Rifle))
		{
			Character->RestBackWeapon();
		}

		Character->GetFPSCharacterStatComponent()->SetCharacterCurrentWeaponNum();
		Character->FPSmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(true);
		Character->Thirdmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(true);


		if (Character->GetPlayerController())
		{
			if (Character->GetPlayerController()->IsLocalController())
			{
				//Character->FPSmesh[uint8(GetFPSCharacterStatComponent()->GetCurrentWeaponNumber()) - 1]->SetVisibility(true);
				if (Character->GetFPSUIWidget())
				{
					Character->GetFPSUIWidget()->SetAmmoCount(this);
					Character->GetFPSCharacterStatComponent()->GetCurrentWeapon()->Draw(this);
				}
			}
		}

		Character->CurrentAnimationWeaponNumber = uint8(Weapon->eGunNumber);
	}
}


void AFPSCharacter::ServerDropedWeapon_Implementation(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo)
{
	ClientDropedWeapon(Character, WeaponNum, Location, Impulse, CurrentAmmo, ElseAmmo);
}

void AFPSCharacter::ClientDropedWeapon_Implementation(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo)
{
	if (!Character->GetFPSCharacterStatComponent()) return;

	AWBase* Weapon;

	switch (WeaponNum)
	{
	case EWeaponNum::E_Grenade:
	case EWeaponNum::E_Flash:
	case EWeaponNum::E_Smoke:
	case EWeaponNum::E_Knife:
		return;

	case EWeaponNum::E_Sub:
	case EWeaponNum::E_Rifle:
	case EWeaponNum::E_C4:
		Weapon = Character->GetFPSCharacterStatComponent()->GetSelectWeapon(WeaponNum);
		break;

	default:
		return;
	}

	if (Weapon)
	{
		if (Character->GetFPSCharacterStatComponent()->GetCurrentWeapon() && Character->GetFPSCharacterStatComponent()->GetCurrentWeapon() == Weapon)
		{
			if (AWGun* DropedGun = Cast<AWGun>(Weapon))
			{
				DropedGun->SetAmmoCount(CurrentAmmo, ElseAmmo);
			}

			if (Character->GetFPSCharacterStatComponent()->GetWeaponArrayNum() == 1)
			{
				Character->FPSmesh[uint8(WeaponNum) - 1]->SetVisibility(false);
				Character->Thirdmesh[uint8(WeaponNum) - 1]->SetVisibility(false);
			}

			Character->DropAndEnableChangeWeapon();


			//Character->GetFPSCharacterStatComponent()->SetCharacterCurrentWeapon(nullptr);

			//if (Weapon->eWeaponNum == EWeaponNum::E_Rifle || Weapon->eWeaponNum == EWeaponNum::E_Sub)
			//{
			//	Character->GetFPSCharacterStatComponent()->SetCharacterCurrentGun(nullptr);
			//}
		}

		// modify..
		//EWeaponNum Num = Character->DropAndEnableChangeWeapon();
		Character->GetFPSCharacterStatComponent()->RemoveSelectWeapon(WeaponNum);

		Weapon->SetOwningPlayer(nullptr);
		Weapon->SetActorHiddenInGame(false);
		Weapon->WeaponComponent->SetSimulatePhysics(true);
		Weapon->WeaponComponent->SetCollisionProfileName(TEXT("Weapon"));
		Weapon->SetActorLocation(Location);
		Weapon->WeaponComponent->AddImpulse(Impulse);
		Weapon->SetPossiblePickWeapon(true);
		
	}
}

void AFPSCharacter::ServerChangeWeapon_Implementation(AFPSCharacter* Character, EWeaponNum ChangeWeaponNum)
{
	ClientChangeWeapon(Character, ChangeWeaponNum);
}

void AFPSCharacter::ClientChangeWeapon_Implementation(AFPSCharacter* Charcter, EWeaponNum ChangeWeaponNum)
{
	if (!Charcter->StatComponent) return;

	AWBase* Weapon = FindWeapon(ChangeWeaponNum);

	if (!Weapon)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Not Found Any Weapon!!!"));

		//Charcter->FPSmesh[uint8(Charcter->StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
		//Charcter->Thirdmesh[uint8(Charcter->StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);

		// 수정 필요
		if (Charcter->StatComponent->GetCurrentWeapon() && 
			!Charcter->FPSmesh[uint8(Charcter->StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->IsVisible())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Put Weapon!!!"));
			Charcter->PutNewWeapon(nullptr);
		}
	}

	else
	{
		if (Charcter->StatComponent->GetCurrentWeapon())
		{
			if (Charcter->StatComponent->GetCurrentWeapon() == Weapon)
			{
				return;
			}

			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Change Weapon!!!"));
				// Clear the custom timer....
				if (Charcter->StatComponent->GetCurrentGunWeapon())
				{
					AWSniperRifle* Sniper = Cast<AWSniperRifle>(Charcter->StatComponent->GetCurrentGunWeapon());
					if (Sniper)
					{
						Sniper->SetSniperScopeState(EScope::NOSCOPE);
						Sniper->CancelScope();
						Sniper->CancelTimer();
					}
					Charcter->StatComponent->GetCurrentGunWeapon()->ResetTimeHandle();
				}
				else
				{
					Charcter->StatComponent->GetCurrentWeapon()->ResetTimeHandle();
				}

				Charcter->FPSmesh[uint8(Charcter->StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
				Charcter->Thirdmesh[uint8(Charcter->StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);

				Charcter->PutNewWeapon(Weapon);
			}
		}
	}
}


void AFPSCharacter::SyncClientSendDamaged_Implementation(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Causer, FVector Direction, float ReviveTime)
{
	ServerGetDamaged(Character, DirectionType, HP, Kevlar, HitType, Causer, Direction, ReviveTime);
}

void AFPSCharacter::ServerGetDamaged_Implementation(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Causer, FVector Direction, float ReviveTime)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Go Method!!!"));
		if (AFPSCharacter* DamagedCharacter = Cast<AFPSCharacter>(Iterator->Get()->GetCharacter()))
		{
			if (DamagedCharacter == Character && DamagedCharacter->GetFPSUIWidget())
			{
				DamagedCharacter->GetFPSUIWidget()->SetDamageUI(DirectionType);
				DamagedCharacter->GetFPSCharacterStatComponent()->SetHP(HP);
				DamagedCharacter->GetFPSCharacterStatComponent()->SetKevlar(Kevlar);
				DamagedCharacter->GetFPSUIWidget()->SetArmorAndHealth(DamagedCharacter);

				if (HP <= 0)
				{
					DamagedCharacter->SyncClientDeath(DamagedCharacter, Direction, HitType, Causer);
					DamagedCharacter->SyncClientRevive(DamagedCharacter, ReviveTime);

					AFPSCharacter* CauserFPS = Cast<AFPSCharacter>(Causer);
					if (CauserFPS)
					{
						CauserFPS->GetFPSCharacterStatComponent()->SetKillCount();
						DamagedCharacter->DoSomethingOnServer(CauserFPS->GetFPSCharacterStatComponent()->GetKillCount(), CauserFPS);
					}
				}
			}
		}
	}
}

void AFPSCharacter::SyncClientDeath_Implementation(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer)
{
	ServerDeathCharacter(DeathCharacter, Direction, HitType, Causer);

	//DeathCharacter->GetFPSCharacterStatComponent()->Death(DeathCharacter, Direction, HitType, Causer);
}

void AFPSCharacter::ServerDeathCharacter_Implementation(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer)
{
	DeathCharacter->GetFPSCharacterStatComponent()->Death(DeathCharacter, Direction, HitType, Causer);
}


void AFPSCharacter::SyncClientRevive_Implementation(AFPSCharacter* ReviveCharacter, float Sec)
{
	ServerCharacterRevive(ReviveCharacter, Sec);
}

void AFPSCharacter::ServerCharacterRevive_Implementation(AFPSCharacter* ReviveCharacter, float Sec)
{
	ReviveCharacter->GetFPSCharacterStatComponent()->Revive(ReviveCharacter, Sec);
}


void AFPSCharacter::DoSomethingOnServer_Implementation(int32 KillCount, AFPSCharacter* CauserCharacter)
{
	KillEvent(KillCount, CauserCharacter);
}


// 블루프린트 변수 가져오기
//void AFPSCharacter::GetBluePrintVariable(FString VariableName)
//{
//	// 변수의 이름으로 검색하여 UProperty 를 가져온다.
//	UProperty* Prop = GetClass()->FindPropertyByName(*VariableName);
//
//	// 현재 클래스의 변수가 맞는지 비교
//	if (Prop->GetClass() == UObjectProperty::StaticClass())
//	{
//		UObjectProperty* objectProp = Cast<UObjectProperty>(Prop);
//
//		// if (objectProp->PropertyClass == 원하는변수의 클래스형태::StaticClass())
//		// 예제 시작
//		if (objectProp->PropertyClass == UWidgetAnimation::StaticClass())
//		{
//			UObject* obj = objectProp->GetObjectPropertyValue_InContainer(this);
//
//			//UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation>(obj);
//			//if (IsValid(WidgetAnim))
//			//{
//			//	// 예제 끝
//			//}
//		}
//	}
//}
