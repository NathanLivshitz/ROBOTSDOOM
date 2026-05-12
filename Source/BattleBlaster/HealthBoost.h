// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerBoost.h"
#include "HealthBoost.generated.h"

/**
 * 
 */
class ATank;
UCLASS()
class BATTLEBLASTER_API AHealthBoost : public APowerBoost
{
	GENERATED_BODY()
protected:
	virtual void ApplyBoost(ATank* Tank) override;
	
};
