// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageBoost.h"

#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGameMode.h"
void ADamageBoost::ApplyBoost(ATank* Tank)
{
	if (!Tank)
	{
		return;
	}

	Tank->ActivateDamageBoost(BoostedDamage, BoostDuration);
	//  Show message
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		GameMode->ShowPickupMessage(TEXT("FULL DAMAGE!"));
	}
}