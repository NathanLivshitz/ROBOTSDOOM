// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "BattleGameMode.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle FireTimerHandle;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsEnemyControlled())
	{
		ABasePawn* ControlledTarget = GetControlledTarget();
		if (ControlledTarget)
		{
			RotateTurret(ControlledTarget->GetActorLocation());
		}
	}
	else
	{

		if (IsInFireRange())
		{
			RotateTurret(Tank->GetActorLocation());
		}

	}
}

void ATower::CheckFireCondition()
{
	if (Tank && Tank->IsAlive && IsInFireRange())
	{
		if (IsFrozen())
		{
			return;
		}

		if (IsEnemyControlled())
		{
			ABasePawn* ControlledTarget = GetControlledTarget();
			if (ControlledTarget && GetDistanceToEnemyActor(ControlledTarget) <= FireRange)
			{
				RotateTurret(ControlledTarget->GetActorLocation());
				Fire();
			}
			return;
		}

		if (Tank && Tank->IsAlive && IsInFireRange())
		{
			RotateTurret(Tank->GetActorLocation());
			Fire();
		}
	}
}

bool ATower::IsInFireRange()
{
	bool Result = false;

	if (Tank)
	{
		float DistanceToTank = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
		Result = (DistanceToTank <= FireRange);
	}

	return Result;
}
void ATower::HandleDestruction()
{
	Super::HandleDestruction();

	Destroy();
}
ABasePawn* ATower::GetControlledTarget() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePawn::StaticClass(), FoundActors);

	ABasePawn* ClosestTarget = nullptr;
	float ClosestDistance = FireRange; // or another variable

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

float ATower::GetDistanceToEnemyActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return MAX_FLT;
	}

	return FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
}