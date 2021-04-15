// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeSetAttackAnimNotify.h"
#include "WKnife.h"
#include "FPSCharacterStatComponent.h"

void UKnifeSetAttackAnimNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);

	player = Cast<AFPSCharacter>(MeshComp->GetOwner());

	if (player)
	{
		Knife = Cast<AWKnife>(player->GetFPSCharacterStatComponent()->GetCurrentWeapon());

		if (Knife)
		{
			Knife->SetHit();
		}
	}
}


FString UKnifeSetAttackAnimNotify::GetNotifyName_Implementation() const
{
	return L"Attack";
}
