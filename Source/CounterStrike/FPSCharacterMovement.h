// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovement.generated.h"



class ACharacter;
class AFPSCharacter;

UCLASS()
class COUNTERSTRIKE_API UFPSCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:

	FTimerHandle DamagedTimer;

	UPROPERTY(EditAnywhere)
		USoundBase* FallSound;

	float speed;

	bool FlagFall = false;
	float StartJumpZValue;

	float speedRatio = 1.f;

	virtual void BeginPlay() override;

	virtual bool ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation);
	virtual void HandleImpact(const FHitResult & Hit, float TimeSlice, const FVector & MoveDelta);

public:


	void ResetSpeedRatio();

	UFPSCharacterMovement();

	void CheckFallDamaged();

	void SetSpeedRatio(float Value) { speedRatio = Value; }
	float GetSpeedRatio() { return speedRatio; }

	float GetStartJumpZValue() { return StartJumpZValue; }

	void SetRunSpeed(float Speed) { RunSpeed = Speed; }

	float GetRunSpeed() { return RunSpeed; }

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void AddImpulse(FVector Impulse, bool bVelocityChange = false);

	void Accelerate(float DeltaTime, FVector WishDir, float WishSpeed, float Accel);
	void PerformMovement(float DeltaTime);
	void MoveComponent(float DeltaTime);
	void TryJump();
	void ApplyCrouch();
	void TryStayOnGround();

	void WalkMove(float DeltaTime);

	void AirMove(float DeltaTime);

	void ApplyFriction(float DeltaTime);

	bool IsOnGround;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flux")
		bool IsGrounded();

	FVector GroundNormal;

	bool JustTeleported;

	FVector InputVector;

	UPROPERTY(BlueprintReadOnly)
	bool IsJumpHeld;

	UPROPERTY(BlueprintReadOnly)
	bool IsCrouchHeld;

	UPROPERTY(BlueprintReadOnly)
		bool IsWalkHeld;


	UPROPERTY(EditAnywhere, Category = "Flux")
		float JumpSpeed = 500;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float Gravity;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float StepSize;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float Friction = 3000;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float MinWalkableZ = 0.7;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float MMass = 50.0;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float RunSpeed = 609.6f;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float WalkAccel = 5000;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float AirSpeed = 50;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float AirAccel = 4000;


	UPROPERTY(EditAnywhere, Category = "Flux")
		float LimitBunnyHop = 800;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float CrouchSpeed = 150;

	UPROPERTY(EditAnywhere, Category = "Flux")
		float WalkSpeed = 250;

};
