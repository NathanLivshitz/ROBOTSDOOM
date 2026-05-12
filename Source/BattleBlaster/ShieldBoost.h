// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerBoost.h"
#include "ShieldBoost.generated.h"

/**
 * 
 */
class ATank;
UCLASS()
class BATTLEBLASTER_API AShieldBoost : public APowerBoost
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Boost")
	float ShieldDuration = 15.f;

	virtual void ApplyBoost(ATank* Tank) override;
};
