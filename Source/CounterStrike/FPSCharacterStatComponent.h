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

	void InitCharacterHP();
	void InitDollar();
	void SetDollar(uint32 Dollar);
	void PurchaseKevlar();
	void PurchaseKavlarAndHelmet();


	void GetDamage(AActor* DamagedActor, int16 Damage, float Penetration, EBoneHit HitType, FVector HitPoint, UParticleSystem* ParticleEffect, FVector Direction = FVector::ZeroVector);
	void CheckDeath(AFPSCharacter* DeadActor, FVector Direction, EBoneHit HitType);

	EKindOfDeath GetDeathNum() { return DeathNum; }


	void Death(AFPSCharacter* DeathActor, FVector Direction, EBoneHit HitType);
	void Revive(AFPSCharacter* ReviveActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:
	bool bIsDead = false;
	EKindOfDeath DeathNum = EKindOfDeath::EKOD_NONE;

	// Move.. Stat Component..
	AWGun* CurrentGunObj = nullptr;
	AWBase* CurrentWeapon = nullptr;
	// my Current Weapon num..
	EWeaponNum eWeaponNum;
	LOWER_STATE low_State;
	UPPER_STATE upper_State;

	// Weapon Array...
	TArray <AWBase*> WeaponArray;

	UPROPERTY(VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int16 CurrentHP;
	UPROPERTY(VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int16 CurrentKevlar;
	UPROPERTY(VisibleAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
		int32 CurrentDollar;
};
