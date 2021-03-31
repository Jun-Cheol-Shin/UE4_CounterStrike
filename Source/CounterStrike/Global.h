// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

#define MULT 0.25f
#define HANDCOUNT 7

UENUM(BlueprintType)
enum class EDamagedDirectionType : uint8
{
	EDDT_ALL = 0				UMETA(DisplayName = "ALL"),
	EDDT_FRONT = 1				UMETA(DisplayName = "FRONT_HIT"),
	EDDT_BACK = 2				UMETA(DisplayName = "BACK_HIT"),
	EDDT_LEFT = 3				UMETA(DisplayName = "LEFT_HIT"),
	EDDT_RIGHT = 4				UMETA(DisplayName = "RIGHT_HIT"),
};

UENUM(BlueprintType)
enum class EKindOfDeath : uint8
{
	EKOD_NONE = 0				UMETA(DisplayName = "DEATH_NONE"),
	EKOD_FRONT = 1				UMETA(DisplayName = "DEATH_FRONT"),
	EKOD_BACK = 2				UMETA(DisplayName = "DEATH_BACK"),
	EKOD_RIGHT = 3				UMETA(DisplayName = "DEATH_RIGHT"),
	EKOD_LEFT = 4				UMETA(DisplayName = "DEATH_LEFT"),
	EKOD_GUTS = 5				UMETA(DisplayName = "DEATH_GUTS"),
	EKOD_HEAD = 6				UMETA(DisplayName = "DEATH_HEAD"),
	EKOD_CROUCH = 7				UMETA(DisplayName = "DEATH_CROUCH"),
};

enum class EParticlePoolList : uint8
{
	EPP_NONE = 0,
	EPP_MUZZLE = 1,
	EPP_BLOOD = 2,
};

UENUM(BlueprintType)
enum class EBoneHit : uint8
{
	EB_NONE = 0			UMETA(DisplayName = "HIT"),
	EB_HEAD = 1			UMETA(DisplayName = "HEAD_HIT"),
	EB_LEG = 2			UMETA(DisplayName = "LEG_HIT"),
	EB_GUTS = 3			UMETA(DisplayName = "GUTS_HIT"),
};

enum class EShellPoolList : uint8 
{
	ESP_NONE = 0,
	ESP_PISTOL = 1,
	ESP_762 = 2,
	ESP_RIFLE = 3,
	ESP_BIGRIFLE = 4,
	ESP_SHOTGUN = 5,
};

UENUM(BlueprintType)
enum class EDecalPoolList : uint8
{
	EDP_NONE = 0				UMETA(DisplayName = "DECAL_NONE"),
	EDP_BULLETHOLE = 1			UMETA(DisplayName = "Bullet_Hole"),
	EDP_BLOOD = 2				UMETA(DisplayName = "Blood"),
	EDP_SCAR = 3				UMETA(DisplayName = "Scar"),
	EDP_EXPLOSION = 4			UMETA(DisplayName = "Explosion"),
};

UENUM(BlueprintType)
enum class ECreatWeaponNum : uint8
{
	EC_Knife = 0				UMETA(DisplayName = "KNIFE"),
	EC_AK = 1					UMETA(DisplayName = "AK47"),
	EC_AWP = 2					UMETA(DisplayName = "AWP"),
	EC_DEAGLE = 3				UMETA(DisplayName = "DEAGLE"),
	EC_GLOCK = 4				UMETA(DisplayName = "GLOCK"),
	EC_GRENADE = 5				UMETA(DisplayName = "GRENADE"),
	EC_M4A1 = 6					UMETA(DisplayName = "M4A1"),
	EC_MAC10 = 7				UMETA(DisplayName = "MAC10"),
	EC_NOVA = 8					UMETA(DisplayName = "NOVA"),
	EC_SCOUT = 9				UMETA(DisplayName = "SCOUT"),
	EC_USP = 10					UMETA(DisplayName = "USP"),
	EC_SMOKE =  11				UMETA(DisplayName = "SMOKE"),
	EC_FLASH = 12				UMETA(DisplayName = "FLASH"),
	EC_C4 = 13					UMETA(DisplayName = "C4"),
};


UENUM(BlueprintType)
enum class EWeaponNum : uint8
{
	E_None  = 0,
	E_Rifle 		UMETA(DisplayName = "Rifle"),
	E_Sub 			UMETA(DisplayName = "Sub"),
	E_Knife 		UMETA(DisplayName = "Knife"),
	E_Grenade 		UMETA(DisplayName = "Grenade"),
	E_Smoke			UMETA(DisplayName = "Smoke"),
	E_Flash			UMETA(DisplayName = "Flash"),
	E_C4			UMETA(DisplayName = "C4"),
};

UENUM(BlueprintType)
enum class EScope : uint8
{
	NOSCOPE	= 0		UMETA(DisplayName = "NoScope"),
	SCOPE			UMETA(DisplayName = "Scope"),
	DOUBLESCOPE		UMETA(DisplayName = "DoubleScope")
};

UENUM(BlueprintType)
enum class LOWER_STATE : uint8
{
	IDLE = 0		UMETA(DisplayName = "Idle"),
	JUMP = 1		UMETA(DisplayName = "Jump"),
	CROUCH = 2		UMETA(DisplayName = "Crouch"),
};

UENUM(BlueprintType)
enum class UPPER_STATE : uint8
{
	IDLE	= 0		UMETA(DisplayName = "Idle"),
	ATTACK	= 1		UMETA(DisplayName = "Attack"),
	ACTION	= 2		UMETA(DisplayName = "Action"),
	DRAW	= 3		UMETA(DisplayName = "Draw"),
	RELOAD  = 4		UMETA(DisplayName = "Reload"),
};

UENUM(BlueprintType)
enum class EGunNumber : uint8
{
	NONE	= 0			UMETA(DisplayName = "None"),
	AR_AK				UMETA(DisplayName = "AK47"),
	CARBINE				UMETA(DisplayName = "Carbine"),
	RIFLE				UMETA(DisplayName = "Rifle"),

	SMG_MP5				UMETA(DisplayName = "MP5"),


	SHOTGUN				UMETA(DisplayName = "Shotgun"),

	MG_M249				UMETA(DisplayName = "M249"),

	ONEHANDED			UMETA(DisplayName = "OneHanded"),
	TWOHANDED			UMETA(DisplayName = "TwoHanded"),

	ETC_KNIFE			UMETA(DisplayName = "Knife"),
	ETC_GRENADE			UMETA(DisplayName = "Grenade"),
	ETC_C4				UMETA(DisplayName = "C4"),
};
