// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "BattleGameMode.h"
ARobotEnemy::ARobotEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	RobotMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RobotMesh"));
	RobotMesh->SetupAttachment(RootComponent);
}

void ARobotEnemy::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		Tank = Cast<ATank>(PlayerPawn);
	}

	GetWorldTimerManager().SetTimer(FireTimerHandle,this,&ARobotEnemy::CheckFireCondition,FireRate,true);
}

void ARobotEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode || !GameMode->AreEnemiesEnabled())
	{
		bIsMoving = false;
		return;
	}
	if (GameMode && GameMode->IsGameOver)
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		return;
	}

	if (IsEnemyControlled())
	{
		ABasePawn* ControlledTarget = GetControlledTarget();
		if (!ControlledTarget)
		{
			bIsMoving = false;
			return;
		}

		RotateTurret(ControlledTarget->GetActorLocation());

		if (!IsFrozen())
		{
			float Distance = GetDistanceToActor(ControlledTarget);

			if (Distance > AcceptanceRadius)
			{
				bIsMoving = true;

				FVector CurrentLocation = GetActorLocation();
				FVector TargetLocation = ControlledTarget->GetActorLocation();

				FVector Direction = TargetLocation - CurrentLocation;
				Direction.Z = 0.f;
				Direction.Normalize();

				FVector NewLocation = CurrentLocation + (Direction * MoveSpeed * DeltaTime);
				SetActorLocation(NewLocation);

				FRotator LookAtRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
				SetActorRotation(LookAtRotation);
			}
			else
			{
				bIsMoving = false;
			}
		}

		return;
	}

	if (!Tank)
	{
		bIsMoving = false;
		return;
	}
	if (!IsTankInDetectionRange())
	{
		return;
	}

	RotateTurret(Tank->GetActorLocation());
	MoveToTank(DeltaTime);
}

void ARobotEnemy::MoveToTank(float DeltaTime)
{
	if (!Tank)
	{
		return;
	}
	if (IsFrozen())
	{
		return;
	}
	float Distance = GetDistanceToTank();

	if (Distance <= AcceptanceRadius)
	{
		bIsMoving = false;
		return;
	}
	bIsMoving = true;
	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Tank->GetActorLocation();

	FVector Direction = TargetLocation - CurrentLocation;
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector NewLocation = CurrentLocation + (Direction * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	FRotator LookAtRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
	SetActorRotation(LookAtRotation);

	UE_LOG(LogTemp, Warning, TEXT("Robot moving = %s"), bIsMoving ? TEXT("true") : TEXT("false"));
}

void ARobotEnemy::CheckFireCondition()
{
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode || !GameMode->AreEnemiesEnabled())
	{
		bIsShooting = false;
		return;
	}
	if (!Tank)
	{
		return;
	}
	if (!IsTankInDetectionRange())
	{
		return;
	}
	if (IsFrozen())
	{
		bIsShooting = false;
		return;
	}

	if (IsEnemyControlled())
	{
		ABasePawn* ControlledTarget = GetControlledTarget();
		if (!ControlledTarget)
		{
			bIsShooting = false;
			return;
		}

		if (GetDistanceToActor(ControlledTarget) <= FireRange)
		{
			bIsShooting = true;
			RotateTurret(ControlledTarget->GetActorLocation());
			Fire();

			GetWorldTimerManager().ClearTimer(ShootAnimationTimerHandle);
			GetWorldTimerManager().SetTimer(ShootAnimationTimerHandle,this,&ARobotEnemy::ResetShootingState,0.25f,false);
		}
		else
		{
			bIsShooting = false;
		}

		return;
	}


	if (GetDistanceToTank() <= FireRange)
	{
		bIsShooting = true;
		Fire();
		GetWorldTimerManager().ClearTimer(ShootAnimationTimerHandle);
		GetWorldTimerManager().SetTimer(ShootAnimationTimerHandle,this,&ARobotEnemy::ResetShootingState,0.25f,false);
	}

	else
	{
		bIsShooting = false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Robot shooting = %s"), bIsShooting ? TEXT("true") : TEXT("false"));
}

float ARobotEnemy::GetDistanceToTank() const
{
	if (!Tank)
	{
		return MAX_FLT;
	}

	return FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
}

void ARobotEnemy::HandleDestruction()
{
	Super::HandleDestruction();

	// later: give coins to player here
	
	Destroy();
}

bool ARobotEnemy::IsTankInDetectionRange() const
{
	if (!Tank)
	{
		return false;
	}

	return FVector::Dist(GetActorLocation(), Tank->GetActorLocation()) <= DetectionRange;
}

void ARobotEnemy::ResetShootingState()
{
	bIsShooting = false;
}

ABasePawn* ARobotEnemy::GetControlledTarget() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePawn::StaticClass(), FoundActors);

	ABasePawn* ClosestTarget = nullptr;
	float ClosestDistance = ControlledTargetSearchRadius;

	for (AActor* Actor : FoundActors)
	{
		ABasePawn* Pawn = Cast<ABasePawn>(Actor);
		if (!Pawn)
		{
			continue;
		}

		if (Pawn == this)
		{
			continue;
		}

		if (Pawn == Tank)
		{
			continue;
		}

		float Distance = FVector::Dist(GetActorLocation(), Pawn->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestTarget = Pawn;
		}
	}

	return ClosestTarget;
}

float ARobotEnemy::GetDistanceToActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return MAX_FLT;
	}

	return FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
}