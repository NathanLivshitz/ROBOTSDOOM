// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "BasePawn.h"

#include "Tank.h"

#include "Tower.generated.h"

/**
 *
 */
UCLASS()
class BATTLEBLASTER_API ATower : public ABasePawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	float FireRange = 1000.0f;

	ATank* Tank;
	UPROPERTY(EditAnywhere)
	float FireRate = 1.0f;

	void CheckFireCondition();
	bool IsInFireRange();
	void HandleDestruction();
private:
	ABasePawn* GetControlledTarget() const;
	float GetDistanceToEnemyActor(AActor* OtherActor) const;
};