// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimNotify.h"


void UFPSCharacterAnimNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);


	//if (!player)
	//{
	//	player = Cast<AFPSCharacter>(MeshComp->GetOwner());
	//}

	//if (player && !Knife)
	//{
	//	Knife = Cast<AWKnife>(player->GetCurrentWeapon());
	//}
}
