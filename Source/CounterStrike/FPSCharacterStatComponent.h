// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WBase.h"
#include "Global.h"
#include "FPSCharacterStatComponent.generated.h"



class AWBase;
class AWGun;
class UParticleSystem;
class USoundBase;

UCLASS()
class COUNTERSTRIKE_API UFPSCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		USoundBase* GunPickupSound;

	UPROPERTY(EditAnywhere)
		USoundBase* DeathSound;

	UPROPERTY(EditAnywhere)
		USoundBase* HeadShotSound;

	UPROPERTY(EditAnywhere)
		USoundBase* KevlarHit;

	UPROPERTY(EditAnywhere)
		USoundBase* HelmetHit;


	// Sets default values for this component's properties
	UFPSCharacterStatComponent();

	uint8 GetCurrentCharacterHP() { return CurrentHP; }
	uint8 GetCurrentCharacterKevlar() { return CurrentKevlar; }
	uint32 GetCurrentCharacterDollar() { return CurrentDollar; }

	// Move Stat Component...
	FORCEINLINE AWBase* GetCurrentWeapon() { return CurrentWeapon; }
	FORCEINLINE EWeaponNum GetCurrentWeaponNumber() { return eWeaponNum; }
	FORCEINLINE UPPER_STATE GetCharacterUpperState() { return upper_State; }
	FORCEINLINE LOWER_STATE GetCharacterLowerState() { return low_State; }
	FORCEINLINE AWGun* GetCurrentGunWeapon() { return CurrentGunObj; }
	FORCEINLINE uint8 GetWeaponArrayNum() { return WeaponArray.Num(); }

	FORCEINLINE TArray <AWBase*> GetWeaponArray() { return WeaponArray; }
	uint8 GetCurrentWeaponNumberInt() { return uint8(eWeaponNum); }

	FORCEINLINE void SetCharacterUpperState(UPPER_STATE state) { upper_State = state; }
	FORCEINLINE void SetCharacterLowerState(LOWER_STATE state) { low_State = state; }
	FORCEINLINE void SetCharacterCurrentWeapon(AWBase* weapon) { CurrentWeapon = weapon; }
	FORCEINLINE void SetCharacterCurrentGun(AWGun* gun) { CurrentGunObj = gun; }
	FORCEINLINE void SetCharacterCurrentWeaponNum() { eWeaponNum = CurrentWeapon->eWeaponNum; }

	void SetArrayWeapon(AWBase* Weapon);
	AWBase* GetSelectWeapon(EWeaponNum Number);
	void RemoveSelectWeapon(EWeaponNum Number);

	UPROPERTY(EditAnywhere)
		uint8 MaxHP = 100;
	UPROPERTY(EditAnywhere)
		uint8 MaxKevlar = 100;
	UPROPERTY(EditAnywhere)
		uint32 MaxDollar = 16000;

	UPROPERTY(EditAnywhere)
		float SprintSpeed = 600.f;

	UPROPERTY(EditAnywhere)
		float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere)
		float CrouchSpeed = 150.f;
	UPROPERTY(EditAnywhere)
		float ReviveTime = 2.f;



	void InitCharacterHP();
	void InitDollar();
	void SetDollar(uint32 Dollar);
	void PurchaseKevlar();
	void PurchaseKavlarAndHelmet();


	EDamagedDirectionType CheckDirection(AFPSCharacter* DamagedActor, FVector Direction);

	void GetDamage(int16 Damage, float Penetration, AFPSCharacter* Causer, EBoneHit HitType = EBoneHit::EB_NONE, FVector Direction = FVector::ZeroVector);
	void CheckDeath(AFPSCharacter* DeadActor, FVector Direction, EBoneHit HitType, AActor* Causer);

	EKindOfDeath GetDeathNum() { return DeathNum; }


	void Death(AFPSCharacter* DeathActor, FVector Direction, EBoneHit HitType, AFPSCharacter* Causer);
	void Revive(AFPSCharacter* ReviveActor, float ReviveSeconds);

	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const;

	FORCEINLINE bool GetIsDead() { return bIsDead; }

	AActorPool* GetActorPool();

	void SetHP(int16 HP) { CurrentHP = HP; }
	void SetKevlar(int16 Kevlar) { CurrentKevlar = Kevlar; }


	UPROPERTY(BlueprintReadWrite)
		bool bGodMode = false;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int32 KillCount = 0;

	UFUNCTION(BlueprintCallable)
		void InitKillCount() { KillCount = 0; }
	UFUNCTION(BlueprintCallable)
		void SetKillCount() { ++KillCount; }
	int32 GetKillCount() { return KillCount; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	FVector RevivePosition;

	AFPSCharacter* Character = nullptr;
	AActorPool* ActorPool;
	bool bIsDead = false;
	
	UPROPERTY(Replicated)
		EKindOfDeath DeathNum = EKindOfDeath::EKOD_NONE;

	// Move.. Stat Component..
	AWGun* CurrentGunObj = nullptr;
	AWBase* CurrentWeapon = nullptr;
	// my Current Weapon num..
	EWeaponNum eWeaponNum;
	LOWER_STATE low_State;
	UPPER_STATE upper_State;

	// Weapon Array...
	UPROPERTY(Replicated)
		TArray <AWBase*> WeaponArray;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int16 CurrentHP;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int16 CurrentKevlar;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int32 CurrentDollar;
};
