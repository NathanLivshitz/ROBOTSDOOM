// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerBoost.h"
#include "DamageBoost.generated.h"

/**
 * 
 */
class ATank;
UCLASS()
class BATTLEBLASTER_API ADamageBoost : public APowerBoost
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostedDamage = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostDuration = 15.f;

	virtual void ApplyBoost(ATank* Tank) override;
};
