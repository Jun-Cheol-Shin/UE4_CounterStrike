// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterMovement.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UFPSCharacterMovement::UFPSCharacterMovement()
{
	CrouchedHalfHeight = 60.f;

	//SetWalkableFloorZ(10.f);
}

// Called when the game starts or when spawned
void UFPSCharacterMovement::BeginPlay()
{
	Super::BeginPlay();
}

bool UFPSCharacterMovement::ResolvePenetrationImpl(const FVector & Adjustment, const FHitResult & Hit, const FQuat & NewRotation)
{
	//UE_LOG(LogTemp, Log, TEXT("ResolveImpl"));
	JustTeleported |= Super::ResolvePenetrationImpl(Adjustment, Hit, NewRotation);
	return JustTeleported;
}

void UFPSCharacterMovement::HandleImpact(const FHitResult & Hit, float TimeSlice, const FVector & MoveDelta)
{
	//UE_LOG(LogTemp, Log, TEXT("HandleImpact"));
}

void UFPSCharacterMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	InputVector = ConsumeInputVector().GetClampedToSize(1.0, 1.0);
	
	PerformMovement(DeltaTime);

		
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Yellow, FString::Printf(TEXT("Velocity : %.1f"), FMath::Sqrt(Velocity.X * Velocity.X + Velocity.Y * Velocity.Y)));

	//UE_LOG(LogTemp, Log, TEXT("Vel: %.1f %.1f %.1f"), Velocity.X, Velocity.Y, Velocity.Z);
}

void UFPSCharacterMovement::Accelerate(float DeltaTime, FVector WishDir, float WishSpeed, float Accel)
{
	// See if we are changing direction a bit
	float ProjectedSpeed = FVector::DotProduct(Velocity, WishDir);

	// Reduce wish speed by the amount of veer.
	float AddSpeed = WishSpeed - ProjectedSpeed;	


	if (Velocity.Size2D() >= LimitBunnyHop)
	{
		AddSpeed = 0;
		Velocity += WishDir;
	}

	// If not going to add any speed, done.
	if (AddSpeed <= 0)
		return;

	// Determine amount of accleration.
	float AccelSpeed = Accel * DeltaTime;

	// Cap at addspeed
	if (AccelSpeed > AddSpeed)
		AccelSpeed = AddSpeed;

	// Adjust velocity.


	Velocity += AccelSpeed * WishDir;
}

void UFPSCharacterMovement::PerformMovement(float DeltaTime)
{
	TryJump();
	ApplyCrouch();

	if (IsGrounded())
	{
		ApplyFriction(DeltaTime);
		WalkMove(DeltaTime);
	}
	else
	{
		if (!FlagFall)
		{
			StartJumpZValue = GetActorLocation().Z;
			FlagFall = true;
		}
		AirMove(DeltaTime);
	}

	MoveComponent(DeltaTime);
	if (IsGrounded())
	{
		TryStayOnGround();
		if (FlagFall)
		{
			CheckFallDamaged();
		}
		
	}
		
}

void UFPSCharacterMovement::CheckFallDamaged()
{
	FlagFall = false;

	if (StartJumpZValue - GetActorLocation().Z >= 280.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fall Damaged!!")); 
		UE_LOG(LogTemp, Warning, TEXT("%.1f, %.1f"), StartJumpZValue, GetActorLocation().Z);
		UGameplayStatics::SpawnSoundAttached(FallSound, GetCharacterOwner()->GetMesh());

		ResetSpeedRatio();
	}
}

void UFPSCharacterMovement::ResetSpeedRatio()
{
	speedRatio = 0.4f;

	GetWorld()->GetTimerManager().SetTimer(DamagedTimer, [this]() {

		GetWorld()->GetTimerManager().ClearTimer(DamagedTimer);
		speedRatio = 1.f;

	}, 1.f, false);
}

void UFPSCharacterMovement::MoveComponent(float DeltaTime)
{
	JustTeleported = false;
	FVector Movement = Velocity * DeltaTime;
	if (IsGrounded())
	{
		Movement.Z = -FVector::DotProduct(GroundNormal, Movement) / GroundNormal.Z;
	}

	if (!Movement.IsNearlyZero())
	{
		FHitResult Hit;

		//MoveUpdatedComponent(Movement, UpdatedComponent->GetComponentRotation(), true);

		SafeMoveUpdatedComponent(Movement, UpdatedComponent->GetComponentRotation(), true, Hit);

		 //If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			if (Hit.Normal.Z >= MinWalkableZ)
			{
				IsOnGround = true;
				GroundNormal = Hit.Normal;
			}

			FVector StartPosition = UpdatedComponent->GetComponentLocation(), EndPosition;
			float AdjustedTime = 1.f - Hit.Time;
			SlideAlongSurface(Movement, AdjustedTime, Hit.Normal, Hit, false);

			if (!FMath::IsNearlyZero(GetCharacterOwner()->GetInputAxisValue("MoveRight")) ||
				!FMath::IsNearlyZero(GetCharacterOwner()->GetInputAxisValue("MoveForward")))
			{
				if (!JustTeleported)
				{
					EndPosition = UpdatedComponent->GetComponentLocation();
					Velocity = (EndPosition - StartPosition) / (AdjustedTime * DeltaTime);
				}
			}

			else 
			{
				// slope sliding...
				float slopeAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(FVector::UpVector, Hit.Normal)));
				if (slopeAngle > GetWalkableFloorAngle())
				{
					if (Velocity.Z <= 0)
					{
						EndPosition = UpdatedComponent->GetComponentLocation();
						Velocity = (EndPosition - StartPosition) / (AdjustedTime * DeltaTime);
					}

					else {
						FVector cross = FVector::CrossProduct(Hit.Normal, FVector::UpVector);
						FVector FallDirection = FVector::CrossProduct(Hit.Normal, FVector::UpVector);
						Velocity += FallDirection;
					}
				}

				else
				{
					EndPosition = UpdatedComponent->GetComponentLocation();
					Velocity = (EndPosition - StartPosition) / (AdjustedTime * DeltaTime);
				}
			}
		}
	}

}

void UFPSCharacterMovement::TryJump()
{
	if (!IsGrounded())
		return;

	if (!IsJumpHeld)
		return;

	Velocity.Z += JumpSpeed;
	IsOnGround = false;
	UE_LOG(LogTemp, Log, TEXT("Jump"));
}

void UFPSCharacterMovement::ApplyCrouch()
{

}

void UFPSCharacterMovement::TryStayOnGround()
{
	FVector StartLocation = UpdatedComponent->GetComponentLocation();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector(0, 0, -10), UpdatedComponent->GetComponentRotation(), true, Hit);
	//UE_LOG(LogTemp, Log, TEXT("Normal: %.1f, bBlock: %i"), Hit.Normal.Z, Hit.IsValidBlockingHit() ? 1 : 0);
	if (!Hit.bBlockingHit)
	{
		UpdatedComponent->SetWorldLocation(StartLocation);
		IsOnGround = false;
	}
	else if (Hit.Normal.Z >= MinWalkableZ)
	{
		GroundNormal = Hit.Normal;
	}
	else
	{
		IsOnGround = false;
	}
	//UE_LOG(LogTemp, Log, TEXT("Ground: %.1f %.1f %.1f"), GroundNormal.X, GroundNormal.Y, GroundNormal.Z);
}


void UFPSCharacterMovement::WalkMove(float DeltaTime)
{
	if (IsWalkHeld)
	{
		if (IsCrouchHeld)
		{
			speed = CrouchSpeed * speedRatio;
		}
		else
		{
			speed = WalkSpeed * speedRatio;
		}
	}

	else if (IsCrouchHeld)
	{
		speed = CrouchSpeed * speedRatio;
	}

	else
	{
		speed = RunSpeed * speedRatio;
	}

	Accelerate(DeltaTime, InputVector, speed, WalkAccel);
}

void UFPSCharacterMovement::AirMove(float DeltaTime)
{
	Accelerate(DeltaTime, InputVector, AirSpeed, AirAccel);
	Velocity.Z += GetGravityZ() * DeltaTime * 1.5f;
}

void UFPSCharacterMovement::ApplyFriction(float DeltaTime)
{
	float Speed = Velocity.Size();
	if (Speed == 0)
		return;
	float Drop = Friction * DeltaTime;
	float NewSpeed = Speed - Drop;

	if (NewSpeed < 0)
		NewSpeed = 0;

	Velocity *= (NewSpeed / Speed);

	//UE_LOG(LogTemp, Log, TEXT("Friction"));
}

bool UFPSCharacterMovement::IsGrounded()
{
	return IsOnGround;
}

void UFPSCharacterMovement::AddImpulse(FVector Impulse, bool bVelocityChange)
{
	UE_LOG(LogTemp, Log, TEXT("IMPULSE ADDED"));
	if (bVelocityChange)
		Velocity += Impulse;
	else
		Velocity += Impulse / MMass;
}