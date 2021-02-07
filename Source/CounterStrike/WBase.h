// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "FPSCharacter.h"
#include "Engine/EngineTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Global.h"

#include "WBase.generated.h"


class AFPSCharacter;
class UAnimSequence;
class AWSniperRifle;
class AActorPool;

UCLASS(meta = (DisableNativeTick))
class COUNTERSTRIKE_API AWBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWBase();
	UAnimSequence* CurrentPlayingAnim;

	UPROPERTY(EditAnywhere)
		bool bCanAutoFire = true;

	UPROPERTY(EditAnywhere)
		FName SocketName;


	UPROPERTY(EditAnywhere)
		UAnimSequence* IdleAnim;

	UPROPERTY(EditAnywhere)
		UAnimSequence* DrawAnim;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AttackAnim;




	UPROPERTY(EditAnywhere)
		EWeaponNum eWeaponNum;
	UPROPERTY(EditAnywhere)
		EGunNumber eGunNumber;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* WeaponComponent;
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* FPSHandComponent;

	void PlayAnim(bool loop);

	float GetAttackDelay() { return DelayTime; }
	float GetAnimAttackEndTime();
	float GetActionDelayTime() { return ActionDelayTime; }

	virtual void ResetTimeHandle();

	FName GetSocketName() { return SocketName; }

	// Left Click...
	virtual void Fire();
	// Right Click...
	virtual void Action();

	virtual void StopFire();

	virtual void StopAction();

	// Put Weapon..
	virtual void Draw(AFPSCharacter* player);

	virtual void Idle();

protected:
	AActorPool* ActorPool;

	float WaitTime = 0.f;
	float TakeWeaponRunSpeed;
	UPROPERTY(EditAnywhere)
		float RunSpeedRatio = 1.0f;
	FTimerHandle TimerHandle;
	FTimerHandle SingleShootHandle;
	//FTimerHandle IdleHandle;

	AFPSCharacter* Player = nullptr;

	UPROPERTY(EditAnywhere)
		float DelayTime = 0.f;
	UPROPERTY(EditAnywhere)
		float Weapondistance;
	UPROPERTY(EditAnywhere)
		float ActionDelayTime = 0.f;
	// Action Weapon... (Right Click)
	//UPROPERTY(EditAnywhere)
	//	bool IsSpecialActionWeapon;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ShuffleShotAnim();

	virtual float GetDrawDelay();

	FHitResult CheckWall(FHitResult StartPoint, FVector Direction, bool bIgnoreMyself);
private:
};
