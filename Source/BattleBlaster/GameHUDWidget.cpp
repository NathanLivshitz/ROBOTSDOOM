// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameHUDWidget::SetCoins(int32 Coins)
{
	if (CoinText)
	{
		CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coins: %d"), Coins)));
	}
}

void UGameHUDWidget::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UGameHUDWidget::SetHealthText(const FString& Text)
{
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(Text));
	}
}