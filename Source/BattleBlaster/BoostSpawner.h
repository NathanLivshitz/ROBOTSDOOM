// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostSpawner.generated.h"
class APowerBoost;
class ATargetPoint;
UCLASS()
class BATTLEBLASTER_API ABoostSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<TSubclassOf<APowerBoost>> BoostClasses;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnInterval = 10.f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	bool bOnlyOneBoostAtATime = true;

	FTimerHandle SpawnTimerHandle;

	UPROPERTY()
	APowerBoost* CurrentSpawnedBoost;

	void SpawnRandomBoost();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
