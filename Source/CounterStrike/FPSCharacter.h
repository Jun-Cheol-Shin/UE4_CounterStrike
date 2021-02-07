// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "FPSHUDWidget.h"
#include "FPSCharacterMovement.h"
#include "Global.h"

#include "FPSCharacter.generated.h"

class APlayerController;

class AWBase;
class AWGun;

class UInputComponent;
class UFPSCharacterMovement;
class UFPSCharacterStatComponent;
class UUserWidget;
class UFPSHUDWidget;
class USpringArmComponent;
class USceneCaptureComponent2D;
class UCameraComponent;
class UFPSCharacterAnimInstance;

UCLASS()
class COUNTERSTRIKE_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	
	float RotatingHipValue;

	UAnimInstance* Instance;
	UFPSCharacterAnimInstance* animInstance;
	EWeaponNum CurrentGrenade = EWeaponNum::E_Knife;
	UInputComponent* InputComponent;
	FVector MoveDirection;
	bool FirstView = true;
	// Rotation hips variable..
	bool rotating = false;
	// Smoothing Crouch Variable...
	float DecBEH;
	float DecCapsHeight;
	float MeshHeight;
	float runningTime;

protected:	
	UPROPERTY(EditAnywhere)
		UFPSCharacterStatComponent* StatComponent;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	//UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "PB Player|Camera")
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate.*/
	//UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "PB Player|Camera")
		float BaseLookUpRate;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CameraShake)
		TSubclassOf<UCameraShake> FPSCameraAttack;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<UFPSHUDWidget> HUDWidgetClass;

	UPROPERTY()
		UUserWidget* HUDWidget;

	UFPSHUDWidget* FPSUIWidget;

public:
	UPROPERTY(EditAnywhere)
		USoundBase* StartSound;

	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly)
		bool IsAttackHeld = false;
	UPROPERTY(BlueprintReadOnly)
		bool IsActionHeld = false;

	bool bIsReloading = false;

	bool bIsFlashBang = false;

	float CurrentLowerHipsRotation = 0.f;
	float LowerHipsRotation;
	float MoveForwardValue;
	float MoveRightValue;
	float AimOffsetPitch;
	float AimOffsetYaw;
	bool AttackAnimCall = false;

	float ReloadStartTime = 0.f;

	float DelayTime;

	FORCEINLINE UFPSHUDWidget* GetFPSUIWidget() { return FPSUIWidget; }
	FORCEINLINE UFPSCharacterStatComponent* GetFPSCharacterStatComponent() { return StatComponent; }
	FORCEINLINE UFPSCharacterMovement* GetFPSCharacterMovement() { return Cast<UFPSCharacterMovement>(GetCharacterMovement()); }
	FORCEINLINE TSubclassOf<class UCameraShake> GetFPSCameraAttack() { return FPSCameraAttack; }
	FORCEINLINE APlayerController* GetPlayerController() { return Cast<APlayerController>(GetController()); }

	void SetUpperState(UPPER_STATE ChangeState);

	// Camera
	UPROPERTY(EditAnywhere)
		UCameraComponent* FPSCameraComponent;

	// Hand
	UPROPERTY(EditAnywhere, Category = Mesh)
		TArray <class USkeletalMeshComponent*> FPSmesh;
	// 3rd Mesh...
	UPROPERTY(EditAnywhere, Category = Mesh)
		TArray <class UStaticMeshComponent*> Thirdmesh;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere)
		USceneCaptureComponent2D* CaptureCamera;

	UPROPERTY(EditAnywhere, Category = Sens)
		float Sensitive;
	UPROPERTY(EditAnywhere)
		float InteractionDistance = 400.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;


	// Camera Change...
	UFUNCTION()
		void ChangeViewCamera();
	// Forward Back Move.
	UFUNCTION()
		void MoveForward(float Value);
	// Left Right Move.
	UFUNCTION()
		void MoveRight(float Value);

	// Jump
	UFUNCTION()
		void StartJump();
	UFUNCTION()
		void StopJump();

	// Crouch
	UFUNCTION()
		void StartCrouch();
	UFUNCTION()
		void StopCrouch();

	// Walk
	UFUNCTION()
		void StartWalk();
	UFUNCTION()
		void StopWalk();

	// ETC..
	UFUNCTION()
		void Shot();
	UFUNCTION()
		void StopShot();

	UFUNCTION()
		void Action();
	UFUNCTION()
		void StopAction();

	UFUNCTION()
		void Interaction();

	UFUNCTION()
		void Swap_Main();
	UFUNCTION()
		void Swap_Sub();
	UFUNCTION()
		void Swap_Knife();
	UFUNCTION()
		void Swap_Granade();
	UFUNCTION()
		void Swap_Bomb();

	UFUNCTION()
		void Drop();
	UFUNCTION()
		void Reload();

	UFUNCTION()
		void Shop();


	//UParticleSystem* GetBloodParticle() { return BloodParticle; }


	bool GetisFirstView() { return FirstView; }

	EBoneHit CheckHit(float ZValue);

	void RestBackWeapon();

	USkeletalMeshComponent* GetCurrentFPSMesh();
	UStaticMeshComponent* GetCurrentThirdMesh();

	void DropWeapon(EWeaponNum WeaponNum);

	void ChangeViewPort(bool ScopeOn);
	FName GetCurrentWeaponNumberName();
	float GetRelevantAnimTime(FName MachineState, FName StateName);

	void SmoothingCrouch(float DeltaTime);
	void ResetLowerHips(float DeltaTime);
	float CaculatingDot();
	void RotatingLowerHips(float DeltaTime);
	void RotatingAimOffset(float DeltaTime);
	void ShakeHand(float DelataTime);


	// Move Stat Component..

	void TakeWeapon(FHitResult Hit);
	void TakeWeapon(AActor* Actor);

	void SetCharacterState();
	bool CreateObject(ECreatWeaponNum Number);
	AWBase* FindWeapon(EWeaponNum FindWeaponNumber);
	void ChangeWeapon(const EWeaponNum ChangeWeaponNumber);
	void SetNewWeaponMesh(AWBase* WeaponActor);
	void PutNewWeapon(AWBase* WeaponActor);
	EWeaponNum DropAndEnableChangeWeapon();

	FString GetStateAsString(UPPER_STATE Value);
	FString GetStateAsString(LOWER_STATE Value);
};
