// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameHUDWidget.generated.h"

/**
 * 
 */
class AProjectile;
class UProgressBar;
class UTextBlock;

UCLASS()
class BATTLEBLASTER_API UGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCoins(int32 Coins);
	UFUNCTION(BlueprintCallable)
	void SetHealthPercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void SetHealthText(const FString& Text);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateProjectileDisplay(EProjectileType SelectedType, int32 ExplosionCount, int32 IceCount, int32 ControlCount);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* HealthText;

	
	
};
