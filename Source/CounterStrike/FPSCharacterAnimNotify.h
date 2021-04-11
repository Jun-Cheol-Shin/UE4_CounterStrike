// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FPSCharacterAnimNotify.generated.h"

/**
 * 
 */
class AFPSCharacter;
class AWKnife;
UCLASS()
class COUNTERSTRIKE_API UFPSCharacterAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	AFPSCharacter* player;
	AWKnife* Knife; 

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
