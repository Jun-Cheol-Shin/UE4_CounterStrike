// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCharacterAnimNotify.h"
#include "KnifeCheckAttackAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API UKnifeCheckAttackAnimNotify : public UFPSCharacterAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
