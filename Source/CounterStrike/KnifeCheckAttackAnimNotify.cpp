// Fill out your copyright notice in the Description page of Project Settings.


#include "KnifeCheckAttackAnimNotify.h"
#include "WKnife.h"
#include "FPSCharacterStatComponent.h"

void UKnifeCheckAttackAnimNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);

	player = Cast<AFPSCharacter>(MeshComp->GetOwner());

	if (player)
	{
		Knife = Cast<AWKnife>(player->GetFPSCharacterStatComponent()->GetCurrentWeapon());

		if (Knife)
		{
			Knife->CheckAttack();
		}
	}
}


FString UKnifeCheckAttackAnimNotify::GetNotifyName_Implementation() const
{
	return L"CheckAttack";
}
