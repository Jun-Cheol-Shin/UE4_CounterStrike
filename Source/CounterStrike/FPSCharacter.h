// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Global.h"
#include "FPSCharacter.generated.h"

class APlayerController;

class AWBase;
class AWGun;
class AActorPool;
class UInputComponent;
class UFPSCharacterMovement;
class UFPSCharacterStatComponent;
class UUserWidget;
class UFPSHUDWidget;
class USpringArmComponent;
class USceneCaptureComponent2D;
class UCameraComponent;
class UFPSCharacterAnimInstance;
class UNiagaraSystem;
class AStaticMeshActor;
class USphereComponent;
class UBoxComponent;

UCLASS()
class COUNTERSTRIKE_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	// HitBox..

	UAnimInstance* Instance;
	UFPSCharacterAnimInstance* animInstance;
	EWeaponNum CurrentGrenade = EWeaponNum::E_Knife;
	//UInputComponent* InputComponent;
	bool FirstView = true;
	// Rotation hips variable..
	bool rotating = false;
	// Smoothing Crouch Variable...
	float DecBEH;
	float DecCapsHeight;
	float MeshHeight;
	float runningTime;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraShake)
		TSubclassOf<UCameraShake> FPSCameraAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
		TSubclassOf<UFPSHUDWidget> HUDWidgetClass;
	UPROPERTY(BlueprintReadWrite)
		UUserWidget* HUDWidget;
	UPROPERTY(BlueprintReadWrite)
		UFPSHUDWidget* FPSUIWidget;

public:

	UPROPERTY(BlueprintReadWrite)
		bool IsPossibleAttack = false;

	AFPSCharacter();
	bool IsJumpHeld = false;
	bool IsAttackHeld = false;
	bool IsActionHeld = false;
	bool bIsFlashBang = false;

	UPROPERTY(Replicated)
		bool IsWalkHeld = false;
	UPROPERTY(Replicated)
		bool IsCrouchHeld = false;

	UPROPERTY(Replicated)
		bool bIsReloading = false;

	UPROPERTY(Replicated)
		float MoveForwardValue = 0.f;
	UPROPERTY(Replicated)
		float MoveRightValue = 0.f;
	//UPROPERTY(Replicated)
		float CurrentLowerHipsRotation = 0.f;

	UPROPERTY(Replicated)
		bool AttackAnimCall = false;

	UPROPERTY(Replicated)
		float ReloadStartTime = 0.f;

	UPROPERTY(Replicated)
		uint8 CurrentAnimationWeaponNumber = 0;

	UPROPERTY(Replicated)
		float DelayTime = 0.f;

	UPROPERTY(Replicated)
		float AimOffsetPitch = 0.f;
	UPROPERTY(Replicated)
		float AimOffsetYaw = 0.f;


	float LowerHipsRotation = 0.f;


	UFUNCTION(BlueprintCallable)
		FORCEINLINE UFPSCharacterStatComponent* GetFPSCharacterStatComponent() { return StatComponent; }
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
		TArray <class USkeletalMeshComponent*> Thirdmesh;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArmComponent;

	/*UPROPERTY(EditAnywhere)
		USceneCaptureComponent2D* CaptureCamera;*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sens)
		float Sensitive;
	UPROPERTY(EditAnywhere)
		float InteractionDistance = 400.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

	// Forward Back Move.
	UFUNCTION(BlueprintCallable)
		void MoveForward(float Value);
	// Left Right Move.
	UFUNCTION(BlueprintCallable)
		void MoveRight(float Value);

	// Jump
	UFUNCTION(BlueprintCallable)
		void StartJump();
	UFUNCTION(BlueprintCallable)
		void StopJump();


	// Crouch
	UFUNCTION(BlueprintCallable)
		void StartCrouch();
	UFUNCTION(BlueprintCallable)
		void StopCrouch();

	// Walk
	UFUNCTION(BlueprintCallable)
		void StartWalk();
	UFUNCTION(BlueprintCallable)
		void StopWalk();

	// ETC..
	UFUNCTION(BlueprintCallable)
		void Shot();
	UFUNCTION(BlueprintCallable)
		void StopShot();

	UFUNCTION(BlueprintCallable)
		void Action();
	UFUNCTION(BlueprintCallable)
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

	UFUNCTION()
		void SetRagdoll();

	//UParticleSystem* GetBloodParticle() { return BloodParticle; }

	AActorPool* GetActorPool();

	bool GetisFirstView() { return FirstView; }

	EBoneHit CheckHit(FString HitBoneName);

	void RestBackWeapon();

	USkeletalMeshComponent* GetCurrentFPSMesh();
	USkeletalMeshComponent* GetCurrentThirdMesh();

	void DropWeapon(EWeaponNum WeaponNum);

	void ChangeViewPort(bool ScopeOn);
	FName GetCurrentWeaponNumberName();
	float GetRelevantAnimTime(FName MachineState, FName StateName);

	void SmoothingCrouch(float DeltaTime);
	void CrouchMeshPos();
	float CaculatingDot(FVector Vector1, FVector Vector2);
	void ResetLowerHips(float DeltaTime);
	void RotatingLowerHips(float DeltaTime);
	void RotatingAimOffset(FRotator Actor, FRotator Control);
	void ShakeHand(float DelataTime);

	UFUNCTION(BlueprintCallable)
		bool PurchaseWeapon(ECreatWeaponNum WeaponNumber);
	UFUNCTION(BlueprintCallable)
		void SetFPSUIHUD(APlayerController* MyController);
	UFUNCTION(BlueprintCallable)
		void ChangeViewCamera(bool Flag);


	// Move Stat Component..

	void TakeWeapon(FHitResult Hit);
	void TakeWeapon(AActor* Actor);

	UFUNCTION()
		bool SetWeaponMethod(AWBase* Weapon, bool Dropping);

	//void SetCharacterState();
	TSubclassOf<AWBase> GetWeaponBP(ECreatWeaponNum Number);
	AWBase* FindWeapon(EWeaponNum FindWeaponNumber);
	void ChangeWeapon(const EWeaponNum ChangeWeaponNumber);

	UFUNCTION()
		EWeaponNum DropAndEnableChangeWeapon();

	void SetNewWeaponMesh(AWBase* WeaponActor);
	void PutNewWeapon(AWBase* WeaponActor);

	FString GetStateAsString(UPPER_STATE Value);
	FString GetStateAsString(LOWER_STATE Value);

	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const;
	UFPSHUDWidget* GetFPSUIWidget();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void DoSomethingOnServer(int32 KillCount, AFPSCharacter* CauserCharacter);
	void DoSomethingOnServer_Implementation(int32 KillCount, AFPSCharacter* CauserCharacter);
	
	UFUNCTION(BlueprintImplementableEvent)
		void KillEvent(int32 KillCount, AFPSCharacter* CauserCharacter);
	
	// Character Animation.....

	UFUNCTION(Server, Reliable)
		void SyncClientSpeed(float Speed);
	void SyncClientSpeed_Implementation(float Speed);


	UFUNCTION(Server, Reliable)
		void SyncClientForward(float Value);
	void SyncClientForward_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void SyncClientRight(float RightValue);
	void SyncClientRight_Implementation(float RightValue);

	UFUNCTION(Server, Reliable)
		void SyncClientAttack(bool flag, float Time);
	void SyncClientAttack_Implementation(bool flag, float Time);

	UFUNCTION(Server, Reliable)
		void SyncClientCrouch(bool flag, float Time);
	void SyncClientCrouch_Implementation(bool flag, float Time);

	UFUNCTION(Server, Reliable)
		void SyncClientWalk(bool flag);
	void SyncClientWalk_Implementation(bool flag);

	UFUNCTION(Server, Reliable)
		void SyncClientReload(bool flag);
	void SyncClientReload_Implementation(bool flag);

	UFUNCTION(Server, Reliable)
		void SyncClientRotation(float Yaw, float Pitch);
	void SyncClientRotation_Implementation(float Yaw, float Pitch);

	UFUNCTION(Server, Reliable)
		void SyncClientActorRotation(FRotator ActorRotation);
	void SyncClientActorRotation_Implementation(FRotator ActorRotation);


	// Effect, Decal Sound.....

	UFUNCTION(Server, Unreliable)
		void SyncClientSpawnMuzzleEffect(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName);
	void SyncClientSpawnMuzzleEffect_Implementation(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnMuzzleEffect(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName);
	void ServerSpawnMuzzleEffect_Implementation(UParticleSystem* MuzzleEffect, USkeletalMeshComponent* MeshComp, FName SocketName);

	UFUNCTION(Server, Unreliable)
		void SyncClientSpawnEffect(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End);
	void SyncClientSpawnEffect_Implementation(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnEffect(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End);
	void ServerSpawnEffect_Implementation(UNiagaraSystem* ParticleEffect, USkeletalMeshComponent* MeshComp, FVector End);

	UFUNCTION(Server, Unreliable)
		void SyncSpawnPenetrateEffect(UNiagaraSystem* ParticleEffect, FVector Start, FVector End);
	void SyncSpawnPenetrateEffect_Implementation(UNiagaraSystem* ParticleEffect, FVector Start, FVector End);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnPenetrateEffect(UNiagaraSystem* ParticleEffect, FVector Start, FVector End);
	void ServerSpawnPenetrateEffect_Implementation(UNiagaraSystem* ParticleEffect, FVector Start, FVector End);


	UFUNCTION(Server, Unreliable)
		void SyncClientSpawnDecal(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot);
	void SyncClientSpawnDecal_Implementation(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnDecal(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot);
	void ServerSpawnDecal_Implementation(TSubclassOf<class ADecalActor> DecalBluePrint, FVector Pos, FRotator Rot);

	UFUNCTION(Server, Unreliable)
		void SyncClientSpawnShell(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp);
	void SyncClientSpawnShell_Implementation(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnShell(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp);
	void ServerSpawnShell_Implementation(TSubclassOf<class AStaticMeshActor> ShellBlueprint, FVector Impulse, USkeletalMeshComponent* MeshComp);


	UFUNCTION(Server, Unreliable)
		void SyncClientSpawnHitEffect(UParticleSystem* BloodParticle, FVector Hit);
	void SyncClientSpawnHitEffect_Implementation(UParticleSystem* BloodParticle, FVector Hit);

	UFUNCTION(NetMulticast, Unreliable)
		void ServerSpawnHitEffect(UParticleSystem* BloodParticle, FVector Hit);
	void ServerSpawnHitEffect_Implementation(UParticleSystem* BloodParticle, FVector Hit);

	UFUNCTION(Server, Reliable)
		void SyncClientSpawnSound(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp);
	void SyncClientSpawnSound_Implementation(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp);

	UFUNCTION(NetMulticast, Reliable)
		void ServerSpawnSound(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp);
	void ServerSpawnSound_Implementation(USoundBase* ShotSound, USkeletalMeshComponent* MeshComp);

	UFUNCTION(Server, Reliable)
		void SyncClientSpawnWeapon(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location);
	void SyncClientSpawnWeapon_Implementation(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnWeapon(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location);
	void MulticastSpawnWeapon_Implementation(AFPSCharacter* Character, TSubclassOf<class AWBase> WeaponBP, FVector Location);

	// New Method.. 3/13 drop.. Equip.. pick up
	UFUNCTION(Server, Reliable)
		void ServerEquipWeapon(AFPSCharacter* Character, AWBase* PickedWeapon);
	void ServerEquipWeapon_Implementation(AFPSCharacter* Character, AWBase* PickedWeapon);

	UFUNCTION(NetMulticast, Reliable)
		void ClientEquipWeapon(AFPSCharacter* Character, AWBase* PickedWeapon);
	void ClientEquipWeapon_Implementation(AFPSCharacter* Character, AWBase* PickedWeapon);

	UFUNCTION(Server, Reliable)
		void ServerTakeoutWeapon(AFPSCharacter* Character, AWBase* Weapon);
	void ServerTakeoutWeapon_Implementation(AFPSCharacter* Character, AWBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
		void ClientTakeoutWeapon(AFPSCharacter* Character, AWBase* Weapon);
	void ClientTakeoutWeapon_Implementation(AFPSCharacter* Character, AWBase* Weapon);

	UFUNCTION(Server, Reliable)
		void ServerDropedWeapon(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo);
	void ServerDropedWeapon_Implementation(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo);

	UFUNCTION(NetMulticast, Reliable)
		void ClientDropedWeapon(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo);
	void ClientDropedWeapon_Implementation(AFPSCharacter* Character, EWeaponNum WeaponNum, FVector Location, FVector Impulse, float CurrentAmmo, float ElseAmmo);

	UFUNCTION(Server, Reliable)
		void ServerChangeWeapon(AFPSCharacter* Charcter, EWeaponNum ChangeWeaponNum);
	void ServerChangeWeapon_Implementation(AFPSCharacter* Character, EWeaponNum ChangeWeaponNum);

	UFUNCTION(NetMulticast, Reliable)
		void ClientChangeWeapon(AFPSCharacter* Charcter, EWeaponNum ChangeWeaponNum);
	void ClientChangeWeapon_Implementation(AFPSCharacter* Charcter, EWeaponNum ChangeWeaponNum);



	// Send Damage... Death .. Revive..
	UFUNCTION(Server, Reliable)
		void SyncClientSendDamaged(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Causer, FVector Direction, float ReviveTime);
	void SyncClientSendDamaged_Implementation(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Cause, FVector Direction, float ReviveTime);

	UFUNCTION(NetMulticast, Reliable)
		void ServerGetDamaged(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Causer, FVector Direction, float ReviveTime);
	void ServerGetDamaged_Implementation(AFPSCharacter* Character, EDamagedDirectionType DirectionType, int16 HP, int16 Kevlar, EBoneHit HitType, AActor* Causer, FVector Direction, float ReviveTime);

	UFUNCTION(Server, Reliable)
		void SyncClientDeath(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer);
	void SyncClientDeath_Implementation(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer);

	UFUNCTION(NetMulticast, Reliable)
		void ServerDeathCharacter(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer);
	void ServerDeathCharacter_Implementation(AFPSCharacter* DeathCharacter, FVector Direction, EBoneHit HitType, AActor* Causer);

	UFUNCTION(Server, Reliable)
		void SyncClientRevive(AFPSCharacter* ReviveCharacter, float Sec);
	void SyncClientRevive_Implementation(AFPSCharacter* ReviveCharacter, float Sec);

	UFUNCTION(NetMulticast, Reliable)
		void ServerCharacterRevive(AFPSCharacter* ReviveCharacter, float Sec);
	void ServerCharacterRevive_Implementation(AFPSCharacter* ReviveCharacter, float Sec);

 };
