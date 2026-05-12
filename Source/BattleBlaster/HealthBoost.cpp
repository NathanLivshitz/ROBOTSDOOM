// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBoost.h"
#include "Tank.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGameMode.h"
void AHealthBoost::ApplyBoost(ATank* Tank)
{
	if (!Tank)
	{
		return;
	}

	UHealthComponent* HealthComponent = Tank->GetHealthComponent();
	if (HealthComponent)
	{
		HealthComponent->HealToFull();
		
	}

	//  Show message
	ABattleGameMode* GameMode =Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		GameMode->ShowPickupMessage(TEXT("FULL HEALTH!"));
	}
}