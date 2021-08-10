// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WGun.h"
#include "WShotGun.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API AWShotGun : public AWGun
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		float ShakingValue;

	UPROPERTY(EditAnywhere)
		UAnimSequence* AfterReload;
	UPROPERTY(EditAnywhere)
		UAnimSequence* InsertShell;


	UPROPERTY(EditAnywhere)
		uint8 ShellBBCount;

	virtual void SpawnShell() override;

public:
	virtual ~AWShotGun()
	{

	}
	AWShotGun();

	virtual void AmmoReload() override;
	virtual void GunShotMethod() override;
	virtual void Reload() override;
	//virtual void RecoilEndVec() override;
	virtual void ShakingCamera() override;
	virtual void ShuffleShotAnim() override;
};
