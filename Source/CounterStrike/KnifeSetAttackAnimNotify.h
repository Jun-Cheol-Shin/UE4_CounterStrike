// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCharacterAnimNotify.h"
#include "KnifeSetAttackAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class COUNTERSTRIKE_API UKnifeSetAttackAnimNotify : public UFPSCharacterAnimNotify
{
	GENERATED_BODY()

public:
	virtual ~UKnifeSetAttackAnimNotify()
	{

	}
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
