// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BattleSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API UBattleSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "High Score")
	int32 BestPhasesSurvived = 0;

	UPROPERTY(BlueprintReadWrite, Category = "High Score")
	float BestSurvivalTime = 0.f;

	
};
