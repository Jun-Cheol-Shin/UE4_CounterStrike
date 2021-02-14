// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSCharacter.h"
#include "Math/Vector.h"

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
#include "GameFramework/PlayerController.h"
#include "FPSCharacterAnimInstance.h"

#include "Components/SceneCaptureComponent2D.h"
#include "GenericPlatform/GenericPlatformMath.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPSCharacterMovement>(ACharacter::CharacterMovementComponentName))
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

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->SetupAttachment(SpringArmComponent);
	FPSCameraComponent->bUsePawnControlRotation = true;

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

		HandMesh->SetRelativeLocation(FVector(0.f, 0, 0.f));
		HandMesh->SetRelativeRotation(FRotator(0, -90, 0));
		HandMesh->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));	


		HandMesh->SetVisibility(false);
		//CaptureCamera->HideComponent(HandMesh);
		FPSmesh.Add(HandMesh);

	}


	for (int i = 0; i < HANDCOUNT; ++i)
	{
		UStaticMeshComponent* WeaponMesh;

		FName MeshName = FName(*FString::Printf(TEXT("WeaponMesh_%d"), i + 1));
		WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(MeshName);
		WeaponMesh->SetupAttachment(GetMesh());
		WeaponMesh->SetOwnerNoSee(true);
		WeaponMesh->SetVisibility(false);
		WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
		WeaponMesh->CanCharacterStepUp(false);
		Thirdmesh.Add(WeaponMesh);

		//CaptureCamera->HideComponent(WeaponMesh);
	}

	GetMesh()->SetOwnerNoSee(true);

	DecBEH = BaseEyeHeight;
	DecCapsHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	//PlayerMovement = GetFPSCharacterMovement();

	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	GetMesh()->SetRelativeLocation(FVector(-25.f, 0, 50));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));
	GetMesh()->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));
	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 0, 65));

	MeshHeight = GetMesh()->GetRelativeLocation().Z;

	GetCharacterMovement()->CrouchedHalfHeight = 60.f;




	static ConstructorHelpers::FClassFinder<UFPSHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/BluePrint/HUD/FPSUI.FPSUI_C'"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	GetCapsuleComponent()->SetCollisionProfileName("Alive");

	SetReplicates(true);
	//bNetUseOwnerRelevancy = true;
	bAlwaysRelevant = true;

	if (GetFPSCharacterMovement())
	{
		GetFPSCharacterMovement()->SetIsReplicated(true);
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnSoundAttached(StartSound, GetMesh());

	IsAttackHeld = false;
	IsActionHeld = false;

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget(GetPlayerController(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(1);
		}
	}

	FPSUIWidget = Cast<UFPSHUDWidget>(HUDWidget);
	if (FPSUIWidget)
	{
		FPSUIWidget->Init(this);
	}

	CreateObject(ECreatWeaponNum::EC_Knife);

	CreateObject(ECreatWeaponNum::EC_GRENADE);
	CreateObject(ECreatWeaponNum::EC_SMOKE);
	CreateObject(ECreatWeaponNum::EC_FLASH);

	CreateObject(ECreatWeaponNum::EC_DEAGLE);
	//CreateObject(ECreatWeaponNum::EC_NOVA);
	//CreateObject(ECreatWeaponNum::EC_MAC10);
	//CreateObject(ECreatWeaponNum::EC_SCOUT);
	//CreateObject(ECreatWeaponNum::EC_AWP);
	CreateObject(ECreatWeaponNum::EC_M4A1);
	//CreateObject(ECreatWeaponNum::EC_AK);

	//CreateObject(ECreatWeaponNum::EC_USP);
	//CreateObject(ECreatWeaponNum::EC_GLOCK);
	//CreateObject(ECreatWeaponNum::EC_DEAGLE);

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
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (StatComponent)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("UpperState : ") + GetStateAsString(StatComponent->GetCharacterUpperState()));
		GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, TEXT("LowerState : ") + GetStateAsString(StatComponent->GetCharacterLowerState()));
	}*/

	SetCharacterState();

	// Server - Client
	RotatingLowerHips(DeltaTime);
	RotatingAimOffset(DeltaTime);

	ResetLowerHips(DeltaTime);

	SmoothingCrouch(DeltaTime);

	ShakeHand(DeltaTime);

	if (FPSUIWidget)
	{
		FPSUIWidget->DynamicCrosshair(this, DeltaTime);
	}

	if (bIsFlashBang)
	{
		FPSUIWidget->FlashBang(this, DeltaTime);
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	InputComponent = PlayerInputComponent;

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

	PlayerInputComponent->BindAction("ViewChange", IE_Pressed, this, &AFPSCharacter::ChangeViewCamera);

	PlayerInputComponent->BindAction("Shot", IE_Pressed, this, &AFPSCharacter::Shot);
	PlayerInputComponent->BindAction("Shot", IE_Released, this, &AFPSCharacter::StopShot);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AFPSCharacter::Action);
	PlayerInputComponent->BindAction("Action", IE_Released, this, &AFPSCharacter::StopAction);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AFPSCharacter::Interaction);

	PlayerInputComponent->BindAction("Swap_Main", IE_Pressed, this, &AFPSCharacter::Swap_Main);
	PlayerInputComponent->BindAction("Swap_Sub", IE_Pressed, this, &AFPSCharacter::Swap_Sub);
	PlayerInputComponent->BindAction("Swap_Knife", IE_Pressed, this, &AFPSCharacter::Swap_Knife);
	PlayerInputComponent->BindAction("Swap_Granade", IE_Pressed, this, &AFPSCharacter::Swap_Granade);
	PlayerInputComponent->BindAction("Swap_Bomb", IE_Pressed, this, &AFPSCharacter::Swap_Bomb);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AFPSCharacter::Drop);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::Reload);

	//PlayerInputComponent->BindAction("Shop", IE_Pressed, this, &AFPSCharacter::Shop);
}

void AFPSCharacter::Shop()
{
	if (!FPSUIWidget) return;

	if (FPSUIWidget->OpenShop())
	{
		if (GetPlayerController())
		{
			GetPlayerController()->bShowMouseCursor = true;
			//GetPlayerController()->bEnableClickEvents = true;
			//GetPlayerController()->bEnableMouseOverEvents = true;
		}
	}

	else
	{
		if (GetPlayerController())
		{
			GetPlayerController()->bShowMouseCursor = false;
			//GetPlayerController()->bEnableClickEvents = false;
			//GetPlayerController()->bEnableMouseOverEvents = false;
		}
	}

}

void AFPSCharacter::Shot()
{
	if (!StatComponent) return;

	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
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

	if (StatComponent->GetCurrentWeapon())
	{
		/*	if (StatComponent->GetCurrentGunWeapon() && StatComponent->GetCurrentGunWeapon()->GetCurrentAmmoCount() == 0)
			{
				if (StatComponent->GetCharacterUpperState() == UPPER_STATE::IDLE)
				{
					StatComponent->GetCurrentGunWeapon()->Reload();
				}
			}*/
		StatComponent->GetCurrentWeapon()->StopFire();
	}
}

void AFPSCharacter::Action()
{
	if (!StatComponent) return;

	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
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

	if (StatComponent->GetCurrentWeapon())
	{
		StatComponent->GetCurrentWeapon()->StopAction();
	}
}

void AFPSCharacter::Drop()
{
	if (!StatComponent) return;

	DropWeapon(StatComponent->GetCurrentWeaponNumber());
}

void AFPSCharacter::Reload()
{
	if (!StatComponent) return;

	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
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
	if (!StatComponent) return;

	FVector Location;
	FRotator Rotation;

	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector End = Location + Rotation.Vector() * InteractionDistance;

	FHitResult Hit;

	bool bSucess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_Visibility);

	if (bSucess)
	{
		TakeWeapon(Hit);
	}

	//DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 5.f, 0, 3.f);
}

void AFPSCharacter::Swap_Main()
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}
	ChangeWeapon(EWeaponNum::E_Rifle);
}

void AFPSCharacter::Swap_Sub()
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}
	ChangeWeapon(EWeaponNum::E_Sub);
}

void AFPSCharacter::Swap_Knife()
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}

	ChangeWeapon(EWeaponNum::E_Knife);
}

void AFPSCharacter::Swap_Granade()
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
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
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}

	ChangeWeapon(EWeaponNum::E_C4);
}

float AFPSCharacter::CaculatingDot()
{
	return FMath::Abs(FVector::DotProduct(GetActorForwardVector(), FPSCameraComponent->GetForwardVector()));
}

void AFPSCharacter::ShakeHand(float DeltaTime)
{

	if (!StatComponent && !StatComponent->GetCurrentWeapon())
		return;


	if (!FMath::IsNearlyZero(GetFPSCharacterMovement()->Velocity.Size2D()))
	{
		FVector vec = GetCurrentFPSMesh()->GetRelativeLocation();
		runningTime += DeltaTime * 5.f;
		if (!GetFPSCharacterMovement()->IsWalkHeld)
		{
			vec.X = FMath::Sin(runningTime) * 1.5f;
		}

		else {
			vec.X = FMath::Sin(runningTime) * 0.7f;
		}
		GetCurrentFPSMesh()->SetRelativeLocation(vec);
	}

	else if((FMath::IsNearlyZero(MoveForwardValue) && 
		FMath::IsNearlyZero(MoveRightValue) && 
		FMath::IsNearlyZero(GetFPSCharacterMovement()->Velocity.Size2D())) 
		|| StatComponent->GetCharacterUpperState() != UPPER_STATE::IDLE)
	{
		runningTime = 0;
		GetCurrentFPSMesh()->SetRelativeLocation(
			FMath::VInterpTo(GetCurrentFPSMesh()->GetRelativeLocation(), FVector(0, 0, 0), DeltaTime, 20));
	}
}

void AFPSCharacter::SetCharacterState()
{

	if (!GetFPSCharacterMovement()->IsGrounded())
	{
		StatComponent->SetCharacterLowerState(LOWER_STATE::JUMP);
	}

	else
	{
		if (GetFPSCharacterMovement()->IsCrouchHeld)
		{
			StatComponent->SetCharacterLowerState(LOWER_STATE::CROUCH);
		}
		else
		{
			StatComponent->SetCharacterLowerState(LOWER_STATE::IDLE);
		}
	}
}

AWBase* AFPSCharacter::FindWeapon(EWeaponNum FindWeaponNumber)
{
	// if find Weapon ret True, else ret False...

	if (!StatComponent) return NULL;

	if (StatComponent->GetWeaponArrayNum() == 0)
		return NULL;
	
	else {
		return StatComponent->GetSelectWeapon(FindWeaponNumber);
	}
}

void AFPSCharacter::ChangeWeapon(const EWeaponNum ChangeWeaponNumber)
{

	if (!StatComponent) return;

	AWBase* Weapon = FindWeapon(ChangeWeaponNumber);

	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Main Weapon!!!"));
		return;
	}

	if (StatComponent->GetCurrentWeapon())
	{
		if (StatComponent->GetCurrentWeapon() == Weapon)
		{
			return;
		}

		else 
		{	
			// Clear the custom timer....
			if (StatComponent->GetCurrentGunWeapon())
			{
				AWSniperRifle* Sniper = Cast<AWSniperRifle>(StatComponent->GetCurrentGunWeapon());
				if (Sniper)
				{
					Sniper->SetSniperScopeState(EScope::NOSCOPE);
					Sniper->CancelScope();
					Sniper->CancelTimer();
				}
				StatComponent->GetCurrentGunWeapon()->ResetTimeHandle();
			}
			else
			{
				StatComponent->GetCurrentWeapon()->ResetTimeHandle();
			}

			FPSmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
			Thirdmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1]->SetVisibility(false);
			

			PutNewWeapon(Weapon);
		}
	}
}

void AFPSCharacter::ChangeViewCamera()
{
	FirstView = !FirstView;


	if (FirstView)
	{
		GetCapsuleComponent()->bHiddenInGame = true;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		SpringArmComponent->TargetArmLength = 0;
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
	if (GetFPSCharacterMovement() && GetFPSCharacterMovement()->IsGrounded())
	{

		GetFPSCharacterMovement()->IsJumpHeld = true;
		StatComponent->SetCharacterLowerState(LOWER_STATE::JUMP);
	}
}


void AFPSCharacter::StopJump()
{
	if (GetFPSCharacterMovement()) {

		GetFPSCharacterMovement()->IsJumpHeld = false;
	}
}


void AFPSCharacter::StartCrouch()
{
	if (!StatComponent) return;
	if (GetFPSCharacterMovement() && GetFPSCharacterMovement()->IsGrounded())
	{
		GetFPSCharacterMovement()->IsCrouchHeld = true;
		StatComponent->SetCharacterLowerState(LOWER_STATE::CROUCH);

		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::RELOAD)
		{
			ReloadStartTime = GetRelevantAnimTime(GetCurrentWeaponNumberName(), TEXT("Reload"));
			//UE_LOG(LogTemp, Warning, TEXT("%.1f"), ReloadStartTime);
		}
	}
}

void AFPSCharacter::StopCrouch()
{
	if (!StatComponent) return;

	if (GetFPSCharacterMovement() && GetFPSCharacterMovement()->IsCrouchHeld) 
	{
		GetFPSCharacterMovement()->IsCrouchHeld = false;

		if (StatComponent->GetCharacterUpperState() == UPPER_STATE::RELOAD)
		{
			ReloadStartTime = GetRelevantAnimTime(GetCurrentWeaponNumberName(), TEXT("Crouch_Reload"));
			//UE_LOG(LogTemp, Warning, TEXT("%.1f"), ReloadStartTime);
		}
	}
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
	//if (!GetFPSCharacterMovement() || GetFPSCharacterMovement()->IsFalling())
	//{
	//	return;
	//}

	const float TargetBEH = GetFPSCharacterMovement()->IsCrouchHeld ? CrouchedEyeHeight: DecBEH;
	const float TargetCapsuleSize = GetFPSCharacterMovement()->IsCrouchHeld ? GetCharacterMovement()->CrouchedHalfHeight : DecCapsHeight;
	const float CurrentMeshHeight = GetFPSCharacterMovement()->IsCrouchHeld ? MeshHeight * 0.45f : MeshHeight;

	if (Controller != NULL)
	{
		if (FirstView) 
		{
			FPSCameraComponent->SetRelativeLocation(FMath::VInterpTo(FPSCameraComponent->GetRelativeLocation(),
				FVector(FPSCameraComponent->GetRelativeLocation().X,
					FPSCameraComponent->GetRelativeLocation().Y,
					TargetBEH), DeltaTime, 13.f));
		}
		GetMesh()->SetRelativeLocation(
			FMath::VInterpTo(
				GetMesh()->GetRelativeLocation(),
				FVector(GetMesh()->GetRelativeLocation().X, GetMesh()->GetRelativeLocation().Y, CurrentMeshHeight), DeltaTime * 4.f, 13.0f));

		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::FInterpTo(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), TargetCapsuleSize, DeltaTime, 13.0f), true);
		// Dist and DeltaMovCaps are used for the interpolation value added to RelativeLocation.Z
		const float Dist = TargetCapsuleSize - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float DeltaMovCaps = Dist * FMath::Clamp<float>(DeltaTime * 13.0f, 0.f, 1.f);
		GetCapsuleComponent()->SetRelativeLocation(
			FMath::VInterpTo(
				GetCapsuleComponent()->GetRelativeLocation(),
				FVector(GetCapsuleComponent()->GetRelativeLocation().X,
					GetCapsuleComponent()->GetRelativeLocation().Y,
					GetCapsuleComponent()->GetRelativeLocation().Z + DeltaMovCaps), DeltaTime, 13.0f), true);
	}
}

void AFPSCharacter::StartWalk()
{
	if (GetFPSCharacterMovement() && !GetFPSCharacterMovement()->IsCrouchHeld) {
		GetFPSCharacterMovement()->IsWalkHeld = true;
	}
}

void AFPSCharacter::StopWalk()
{
	if (!StatComponent) return;

	if (GetFPSCharacterMovement()) {

		GetFPSCharacterMovement()->IsWalkHeld = false;
	}
}

void AFPSCharacter::SetUpperState(UPPER_STATE ChangeState)
{
	if (!StatComponent) return;

	StatComponent->SetCharacterUpperState(ChangeState);
}

bool AFPSCharacter::CreateObject(ECreatWeaponNum Number)
{
	FName Path = "";

	switch (Number)
	{
	case ECreatWeaponNum::EC_Knife:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Knife.BP_Knife_C'";
		break;
	case ECreatWeaponNum::EC_AK:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_AK47.BP_AK47_C'";
		break;
	case ECreatWeaponNum::EC_AWP:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Awp.BP_Awp_C'";
		break;
	case ECreatWeaponNum::EC_DEAGLE:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Deagle.BP_Deagle_C'";
		break;
	case ECreatWeaponNum::EC_GLOCK:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Glock18.BP_Glock18_C'";
		break;
	case ECreatWeaponNum::EC_GRENADE:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Grenade.BP_Grenade_C'";
		break;
	case ECreatWeaponNum::EC_SMOKE:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Smoke.BP_Smoke_C'";
		break;
	case ECreatWeaponNum::EC_FLASH:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Flash.BP_Flash_C'";
		break;
	case ECreatWeaponNum::EC_M4A1:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_M4A1.BP_M4A1_C'";
		break;
	case ECreatWeaponNum::EC_MAC10:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Mac10.BP_Mac10_C'";
		break;
	case ECreatWeaponNum::EC_NOVA:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Nova.BP_Nova_C'";
		break;
	case ECreatWeaponNum::EC_SCOUT:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Scout.BP_Scout_C'";
		break;
	case ECreatWeaponNum::EC_USP:
		Path = "Blueprint'/Game/BluePrint/Weapon/BP_Usp.BP_Usp_C'";
		break;
	}

	if (Path == "")
	{
		return false;
	}

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));
	FTransform transform = FTransform::Identity;

	//AActor* Actor = GetWorld()->SpawnActor<AActor>(GeneratedBP, transform);
	//AWBase* WBaseObj = Cast<AWBase>(Actor);

	AWBase* WBaseObj = GetWorld()->SpawnActor<AWBase>(GeneratedBP, transform);
	WBaseObj->WeaponComponent->SetSimulatePhysics(false);

	if (WBaseObj)
	{
		//DropAndEnableChangeWeapon();
		SetNewWeaponMesh(WBaseObj);

		if (!StatComponent->GetCurrentWeapon())
		{
			PutNewWeapon(WBaseObj);
		}

		RestBackWeapon();

		return true;
	}

	else
	{
		return false;
	}
}

void AFPSCharacter::SetNewWeaponMesh(AWBase * WeaponActor)
{
	if (!StatComponent) return;

	uint8 num = uint8(WeaponActor->eWeaponNum);


	if (StatComponent->GetSelectWeapon(WeaponActor->eWeaponNum))
	{
		StatComponent->RemoveSelectWeapon(WeaponActor->eWeaponNum);
	}

	StatComponent->SetArrayWeapon(WeaponActor);
	//WeaponArray[num - 1] = WeaponActor;

	FPSmesh[num - 1]->SetSkeletalMesh(WeaponActor->FPSHandComponent->SkeletalMesh);
	FPSmesh[num - 1]->SetCastInsetShadow(false);
	FPSmesh[num - 1]->SetCastShadow(false);
	Thirdmesh[num - 1]->SetStaticMesh(WeaponActor->WeaponComponent->GetStaticMesh());

	// need Modify...
	Thirdmesh[num - 1]->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponActor->GetSocketName());

	WeaponActor->SetActorHiddenInGame(true);
	//WeaponActor->SetActorEnableCollision(false);
}

void AFPSCharacter::PutNewWeapon(AWBase* WeaponActor)
{
	if (!StatComponent) return;

	StatComponent->SetCharacterCurrentWeapon(WeaponActor);
	//CurrentWeapon = WeaponActor;

	switch (StatComponent->GetCurrentWeapon()->eWeaponNum)
	{
	case EWeaponNum::E_Rifle:
		StatComponent->SetCharacterCurrentGun(Cast<AWGun>(WeaponActor));
		GetCurrentThirdMesh()->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponActor->GetSocketName());
		break;
	case EWeaponNum::E_Sub:
		StatComponent->SetCharacterCurrentGun(Cast<AWGun>(WeaponActor));
		RestBackWeapon();
		break;
	case EWeaponNum::E_Knife:
	case EWeaponNum::E_C4:
		StatComponent->SetCharacterCurrentGun(nullptr);
		RestBackWeapon();
		break;
	case EWeaponNum::E_Grenade:
	case EWeaponNum::E_Flash:
	case EWeaponNum::E_Smoke:
		StatComponent->SetCharacterCurrentGun(nullptr);
		RestBackWeapon();
		break;
	}

	if (StatComponent->GetCurrentGunWeapon())
	{
		StatComponent->GetCurrentGunWeapon()->SetCamShake(FPSCameraAttack);
	}

	StatComponent->SetCharacterCurrentWeaponNum();
	//eWeaponNum = WeaponActor->eWeaponNum;
	FPSmesh[uint8(StatComponent->GetCurrentWeaponNumber()) - 1]->SetVisibility(true);
	Thirdmesh[uint8(StatComponent->GetCurrentWeaponNumber()) - 1]->SetVisibility(true);

	if (FPSUIWidget)
	{
		FPSUIWidget->SetAmmoCount(this);
	}
	StatComponent->GetCurrentWeapon()->Draw(this);
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
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Sub))
		{
			ChangeWeapon(EWeaponNum::E_Sub);
		}
		else 
		{
			ChangeWeapon(EWeaponNum::E_Knife);
		}
		return EWeaponNum::E_Rifle;

	case EWeaponNum::E_Sub:
		if (StatComponent->GetSelectWeapon(EWeaponNum::E_Rifle))
		{
			ChangeWeapon(EWeaponNum::E_Rifle);
		}
		else
		{
			ChangeWeapon(EWeaponNum::E_Knife);
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
		else
		{
			ChangeWeapon(EWeaponNum::E_Knife);
		}
		return EWeaponNum::E_C4;
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
		if (AWBase* Weapon = Cast<class AWBase>(Hit.GetActor()))
		{
			if (Weapon->eWeaponNum == EWeaponNum::E_Rifle || Weapon->eWeaponNum == EWeaponNum::E_Sub || Weapon->eWeaponNum == EWeaponNum::E_C4)
			{
				if (!StatComponent->GetSelectWeapon(Weapon->eWeaponNum))
				{
					SetNewWeaponMesh(Weapon);

					FPSmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);
					Thirdmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);


					if (!StatComponent->GetCurrentWeapon())
					{
						PutNewWeapon(Weapon);
					}
				}

				else
				{
					DropWeapon(Weapon->eWeaponNum);
					SetNewWeaponMesh(Weapon);

					FPSmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);
					Thirdmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);

					if (!StatComponent->GetCurrentWeapon())
					{
						PutNewWeapon(Weapon);
					}
				}
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
			if (Weapon->eWeaponNum == EWeaponNum::E_Rifle || Weapon->eWeaponNum == EWeaponNum::E_Sub || Weapon->eWeaponNum == EWeaponNum::E_C4)
			{
				if (!StatComponent->GetSelectWeapon(Weapon->eWeaponNum))
				{
					SetNewWeaponMesh(Weapon);

					FPSmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);
					Thirdmesh[uint8(Weapon->eWeaponNum) - 1]->SetVisibility(false);

					if (!StatComponent->GetCurrentWeapon())
					{
						PutNewWeapon(Weapon);
					}
				}
			}
		}
	}
}

USkeletalMeshComponent* AFPSCharacter::GetCurrentFPSMesh()
{
	if (StatComponent)
	{
		return FPSmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1];
	}

	return nullptr;
}

UStaticMeshComponent* AFPSCharacter::GetCurrentThirdMesh()
{
	if (StatComponent)
	{
		return Thirdmesh[uint8(StatComponent->GetCurrentWeapon()->eWeaponNum) - 1];
	}

	return nullptr;
}

void AFPSCharacter::DropWeapon(EWeaponNum WeaponNum)
{

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
		Weapon = StatComponent->GetSelectWeapon(WeaponNum);
		break;

	default:
		Weapon = StatComponent->GetSelectWeapon(StatComponent->GetCurrentWeaponNumber());
		break;
	}

	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);
	Location += FPSCameraComponent->GetForwardVector() * 100.f;

	Weapon->WeaponComponent->SetSimulatePhysics(true);

	Weapon->SetActorLocation(Location);
	Weapon->SetActorHiddenInGame(false);

	Weapon->WeaponComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Weapon->WeaponComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Weapon->WeaponComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	Weapon->WeaponComponent->AddImpulse(FPSCameraComponent->GetForwardVector() *
		500.f * Weapon->WeaponComponent->GetMass() + FPSCameraComponent->GetUpVector() * 100.f);

	EWeaponNum Num = DropAndEnableChangeWeapon();

	StatComponent->RemoveSelectWeapon(Weapon->eWeaponNum);
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

EBoneHit AFPSCharacter::CheckHit(float ZValue)
{
	if (GetFPSCharacterMovement()->IsCrouchHeld)
	{
		if (ZValue > 20)
		{
			return EBoneHit::EB_HEAD;
		}

		else if (ZValue < -20)
		{
			return EBoneHit::EB_LEG;
		}

		else
		{
			return EBoneHit::EB_NONE;
		}
	}

	else
	{
		if (ZValue > 50)
		{
			return EBoneHit::EB_HEAD;
		}

		else if (ZValue < 1)
		{
			return EBoneHit::EB_LEG;
		}

		else
		{
			return EBoneHit::EB_NONE;
		}
	}
}

void AFPSCharacter::MoveForward(float Value)
{

	MoveForwardValue = Value;

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
}

void AFPSCharacter::MoveRight(float Value)
{
	MoveRightValue = Value;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		MoveForwardValue = 1.f;
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
}

void AFPSCharacter::AddControllerYawInput(float Val)
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}

	Val = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	float mulvalue = Val * Sensitive;

	Super::AddControllerYawInput(mulvalue);

	if (CaculatingDot() <= 0.05f)
	{
		rotating = true;
		//RotatingHipValue = GetController()->GetControlRotation().Yaw;
	}
}

void AFPSCharacter::AddControllerPitchInput(float Val)
{
	if (FPSUIWidget)
	{
		if (FPSUIWidget->GetbIsShopOpen())
		{
			return;
		}
	}

	Val = Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

	float mulvalue = Sensitive * Val;

	Super::AddControllerPitchInput(mulvalue);
}

// Aim offset..
void AFPSCharacter::RotatingAimOffset(float DeltaTime)
{
	FRotator offset = FRotator(AimOffsetPitch, AimOffsetYaw, 0);
	FRotator offsetResult = FMath::RInterpTo(offset, GetActorRotation() - GetControlRotation(), DeltaTime, 10);

	AimOffsetYaw = FMath::ClampAngle(offsetResult.Yaw, -90, 90);
	AimOffsetPitch = FMath::ClampAngle(offsetResult.Pitch, -90, 90);
}

// While Moving.. Rotating Lower Hip Replication..

void AFPSCharacter::RotatingLowerHips(float DeltaTime)
{
	if (!FMath::IsNearlyZero(MoveRightValue) && FMath::IsNearlyZero(GetInputAxisValue("MoveForward")))
	{
		LowerHipsRotation = MoveRightValue * 60.f;
		//LowerHipsRotation *= MoveForwardValue;
	}

	else
	{
		LowerHipsRotation = MoveRightValue * 35.f;
	}

	CurrentLowerHipsRotation = FMath::FInterpTo(CurrentLowerHipsRotation, LowerHipsRotation, DeltaTime, 5);
}
// .........
void AFPSCharacter::ResetLowerHips(float DeltaTime)
{
	if (!rotating)
		return;

	GetController()->GetPawn()->SetActorRotation(
		FMath::RInterpTo(GetController()->GetPawn()->GetActorRotation(),
			FRotator(0, GetController()->GetControlRotation().Yaw, 0),
			DeltaTime, 20));


	if (GetController()->GetPawn()->GetActorRotation().Equals(FRotator(0, GetController()->GetControlRotation().Yaw, 0), 0.1f))
	{
		rotating = false;
	}
}


//void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//DOREPLIFETIME(AFPSCharacter, Owner);
//	//DOREPLIFETIME_ACTIVE_OVERRIDE(AFPSCharacter, CharacterMovement, bReplicateMovement);
//}