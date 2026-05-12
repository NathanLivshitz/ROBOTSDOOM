// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostSpawner.h"
#include "PowerBoost.h"
#include "TimerManager.h"
#include "Engine/World.h"
// Sets default values
ABoostSpawner::ABoostSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABoostSpawner::BeginPlay()
{

	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle,this,&ABoostSpawner::SpawnRandomBoost,SpawnInterval,true);
}

// Called every frame
void ABoostSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoostSpawner::SpawnRandomBoost()
{
	if (bOnlyOneBoostAtATime && CurrentSpawnedBoost && IsValid(CurrentSpawnedBoost))
	{
		return;
	}

	if (BoostClasses.Num() == 0 || SpawnPoints.Num() == 0)
	{
		return;
	}

	int32 RandomBoostIndex = FMath::RandRange(0, BoostClasses.Num() - 1);
	int32 RandomSpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

	TSubclassOf<APowerBoost> SelectedBoostClass = BoostClasses[RandomBoostIndex];
	AActor* SelectedSpawnPoint = SpawnPoints[RandomSpawnIndex];

	if (!SelectedBoostClass || !SelectedSpawnPoint)
	{
		return;
	}

	FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation();
	FRotator SpawnRotation = SelectedSpawnPoint->GetActorRotation();

	APowerBoost* SpawnedBoost = GetWorld()->SpawnActor<APowerBoost>(SelectedBoostClass,SpawnLocation,	SpawnRotation);

	if (SpawnedBoost)
	{
		CurrentSpawnedBoost = SpawnedBoost;
	}
}