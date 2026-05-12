// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerBoost.h"
#include "SpeedBoost.generated.h"

/**
 * 
 */
class ATank;
UCLASS()
class BATTLEBLASTER_API ASpeedBoost : public APowerBoost
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostedMoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostDuration = 5.f;

	virtual void ApplyBoost(ATank* Tank) override;
};
