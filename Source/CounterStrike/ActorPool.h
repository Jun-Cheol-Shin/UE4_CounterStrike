// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Global.h"
#include "ActorPool.generated.h"



class AWBase;
class ADecalActor;
class AStaticMeshActor;
class AFPSAICharacter;


//USTRUCT(BlueprintType)
template <typename T> struct FPoolData
{
public:
	TArray<T>	ArrayList;
	TQueue<T>	UseList;
};


UCLASS()
class COUNTERSTRIKE_API AActorPool : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		USoundBase* ShellSound;

	UPROPERTY(EditAnywhere)
		float DecalLifeTimeSpan = 3.f;
	UPROPERTY(EditAnywhere)
		float ShellLifeTimeSpan = 0.5f;

	UPROPERTY(EditAnywhere)
		uint8 DecalCount = 100;

	UPROPERTY(EditAnywhere)
		uint8 ShellCount = 100;

	TMap<FString, FPoolData<ADecalActor*>*> DecalPool;

	TMap<FString, FPoolData<AStaticMeshActor*>*> ShellPool;

	TMap<FString, FPoolData<AFPSAICharacter*>*> AIPool;

public:
	AActorPool() {};

	virtual void BeginPlay() override;
	void Initialized();



	float GetDecalLifeTime() { return DecalLifeTimeSpan; }
	float GetShellLifeTime() { return ShellLifeTimeSpan; }

		// Decal Methods...
	ADecalActor* GetDecal(EDecalPoolList Type);
	void ReturnDecal(ADecalActor* Decal, EDecalPoolList Type);
	void CreateDecal(FName Path, FString ArrayName);

	// Shell Methods...
	void CreateStaticMesh(FName Path, FString ArrayName);
	AStaticMeshActor* GetMesh(EShellPoolList Type);
	void ReturnMesh(AStaticMeshActor* Actor, EShellPoolList Type);

};
