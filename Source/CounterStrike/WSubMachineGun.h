// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WRifle.h"
#include "WSubMachineGun.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AWSubMachineGun : public AWRifle
{
	GENERATED_BODY()
	

public:
	virtual ~AWSubMachineGun()
	{

	}
	AWSubMachineGun();
	virtual void ChangeRecoilDirection() override;
	virtual void SpawnShell() override;
};
