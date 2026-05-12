// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldBoost.h"

#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGameMode.h"
void AShieldBoost::ApplyBoost(ATank* Tank)
{
	if (!Tank)
	{
		return;
	}

	Tank->ActivateShieldBoost(ShieldDuration);

	//  Show message
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		GameMode->ShowPickupMessage(TEXT("SHIELD ACTIVE!"));
	}
}