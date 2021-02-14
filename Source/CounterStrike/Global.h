// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

#define MULT 0.25f
#define HANDCOUNT 7

enum class EDamagedDirectionType : uint8
{
	EDDT_ALL = 0,
	EDDT_FRONT = 1,
	EDDT_BACK = 2,
	EDDT_LEFT = 3,
	EDDT_RIGHT = 4,
};

enum class EKindOfDeath : uint8
{
	EKOD_NONE = 0,
	EKOD_FRONT = 1,
	EKOD_BACK = 2,
	EKOD_RIGHT = 3,
	EKOD_LEFT = 4,
	EKOD_GUTS = 5,
	EKOD_HEAD = 6,
	EKOD_CROUCH = 7,
};

enum class EParticlePoolList : uint8
{
	EPP_NONE = 0,
	EPP_MUZZLE = 1,
	EPP_BLOOD = 2,
};

enum class EBoneHit : uint8 
{
	EB_NONE = 0,
	EB_HEAD = 1,
	EB_LEG = 2,
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


enum class EDecalPoolList : uint8
{
	EDP_NONE = 0,
	EDP_BULLETHOLE = 1,
	EDP_BLOOD = 2,
	EDP_SCAR = 3,
	EDP_EXPLOSION = 4,
};

enum class ECreatWeaponNum : uint8
{
	EC_Knife = 0,
	EC_AK = 1,
	EC_AWP = 2,
	EC_DEAGLE = 3,
	EC_GLOCK = 4,
	EC_GRENADE = 5,
	EC_M4A1 = 6,
	EC_MAC10 = 7,
	EC_NOVA = 8,
	EC_SCOUT = 9,
	EC_USP = 10,
	EC_SMOKE =  11,
	EC_FLASH = 12,
	EC_C4 = 13,
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
	NOSCOPE = 0		UMETA(DisplayName = "NoScope"),
	SCOPE			UMETA(DisplayName = "Scope"),
	DOUBLESCOPE		UMETA(DisplayName = "DoubleScope")
};



enum class LOWER_STATE : uint8
{
	IDLE = 0		UMETA(DisplayName = "Idle"),
	JUMP = 1		UMETA(DisplayName = "Jump"),
	CROUCH = 2		UMETA(DisplayName = "Crouch"),
};

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
