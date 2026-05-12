// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedBoost.h"
#include "Tank.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGameMode.h"
void ASpeedBoost::ApplyBoost(ATank* Tank)
{
	if (!Tank)
	{
		return;
	}

	Tank->ActivateSpeedBoost(BoostedMoveSpeed, BoostDuration);

	//  Show message
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		GameMode->ShowPickupMessage(TEXT("SPEED BOOST!"));
	}
}