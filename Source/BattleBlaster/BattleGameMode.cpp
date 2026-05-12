// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "Tank.h"
#include "BattleBlasterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "RobotEnemy.h"
#include "Engine/TargetPoint.h"
#include "ExploderRobot.h"
#include "EnemySpawnPoint.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();
	LoadHighScore();
	UBattleBlasterGameInstance* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance());
	bool bSkipMenu = false;
	if (GI)
	{
		bSkipMenu = GI->bSkipStartMenu;
		GI->bSkipStartMenu = false;
	}

	bEnemiesEnabled = false;
	
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		Tank = Cast<ATank>(PlayerPawn);
		if (!Tank)
		{
			UE_LOG(LogTemp, Display, TEXT("GameMode: Failed to find the tank actor!"));
		}
	}
	CacheSpawnPoints();
	UE_LOG(LogTemp, Display, TEXT("Start of the loop!"));

	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
	}

	if (bSkipMenu)
	{
		// run the same flow as pressing Play, but next tick
		GetWorldTimerManager().SetTimerForNextTick(this, &ABattleGameMode::StartGame);
		return;
	}

	
		if (Tank)
		{
			Tank->CreateHUDIfNeeded();
			Tank->ShowHUD(true);
			Tank->SetPlayerEnabled(false);
			Tank->UpdateHealthUI();
			Tank->UpdateCoins(Coins);
		}

		if (StartMenuWidgetClass && PlayerController)
		{
			StartMenuWidget = CreateWidget<UUserWidget>(PlayerController, StartMenuWidgetClass);
			if (StartMenuWidget)
			{
				StartMenuWidget->AddToViewport(100);

				FInputModeUIOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
			}
		}
	
	
		if (GI)
		{
			GI->bSkipStartMenu = false;
		}

		if (Tank)
		{
			Tank->CreateHUDIfNeeded();
			Tank->ShowHUD(true);
			Tank->SetPlayerEnabled(false);
			Tank->UpdateHealthUI();
			Tank->UpdateCoins(Coins);
		}

		if (PlayerController)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
			//PlayerController->bShowMouseCursor = false;
			PlayerController->bShowMouseCursor = true;
			PlayerController->bEnableClickEvents = true;
			PlayerController->bEnableMouseOverEvents = true;
		}

		CountdownSeconds = CountdownDelay;
		GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&ABattleGameMode::OnCountdownTimerTimeout,1.0f,true);
	

	

	//CountdownSeconds = CountdownDelay;
	//GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleGameMode::OnCountdownTimerTimeout, 1.0f, true);
}

void ABattleGameMode::OnCountdownTimerTimeout()
{

	
	if (!ScreenMessageWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("ScreenMessageWidget is null in OnCountdownTimerTimeout"));
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		return;
	}

	CountdownSeconds -= 1;

	if (CountdownSeconds > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("About to set countdown text"));
		ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownSeconds));
	}
	else if (CountdownSeconds == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("About to set Go text"));
		ScreenMessageWidget->SetMessageText(TEXT("Go!"));

		if (Tank)
		{
			Tank->SetPlayerEnabled(true);
		}
		bEnemiesEnabled = true;
		StartPhase();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ABattleGameMode::ActorDied(AActor* DeadActor)
{
	
	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		IsGameOver = true;
	}
	else
	{
		ABasePawn* DeadPawn = Cast<ABasePawn>(DeadActor);
		if (DeadPawn)
		{
			DeadPawn->HandleDestruction();
			//EnemyCount--;
			EnemiesAlive--;
			int32 Reward = DeadPawn->GetCoinReward();
			if (DeadPawn->ShouldGiveCoins())
			{
				AddCoins(Reward);
			}//if tower died add  coins to player

			UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed! Player earned 5 coins."));
		
			if (EnemiesAlive <= 0)
			{
				bEnemiesEnabled = false;
				PhasesSurvived++;
				if (Tank)
				{
					Tank->SetPlayerEnabled(false);
				}
				if (ScreenMessageWidget)
				{
					ScreenMessageWidget->SetMessageText(TEXT("PHASE COMPLETE!"));
					ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
				}
				GetWorldTimerManager().SetTimer(PhaseMessageTimerHandle,this,&ABattleGameMode::OpenShop,2.0f,false);
				
			}
			
			
		}
	}

	if (IsGameOver)
	{

		SurvivalTime = GetWorld()->GetTimeSeconds() - RunStartTime;
		SaveHighScoreIfNeeded();

		FString GameOverString = IsVictory ? "Victory!" : "Defeat!";

		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetMessageText(GameOverString);
			ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		}

		// stop countdown just in case
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		SurvivalTime = GetWorld()->GetTimeSeconds() - RunStartTime;

		if (PhasesSurvived > HighScorePhases)
		{
			HighScorePhases = PhasesSurvived;
		}

		// show death menu after delay
		FTimerHandle DeathMenuTimerHandle;
		GetWorldTimerManager().SetTimer(DeathMenuTimerHandle,this,&ABattleGameMode::ShowDeathMenu,5.0f,false);
	}
}

void ABattleGameMode::OnGameOverTimerTimeout()
{
	
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);
		if (BattleBlasterGameInstance)
		{
			if (IsVictory)
			{
				// Load the next level
				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				// Reload the current level
				
				BattleBlasterGameInstance->RestartCurrentLevel();
			}
		}
	}
}
//display massage
void ABattleGameMode::ShowPickupMessage(const FString& Message)
{
	if (ScreenMessageWidget)
	{
		ScreenMessageWidget->SetMessageText(Message);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

//coin system
void ABattleGameMode::AddCoins(int32 Amount)
{
	Coins += Amount;

	UE_LOG(LogTemp, Warning, TEXT("Coins added: %d | Total Coins: %d"), Amount, Coins);

	ATank* PlayerTank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerTank)
	{
		PlayerTank->UpdateCoins(Coins);
	}
}


int32 ABattleGameMode::GetCoins() const

{
	return Coins;
}

bool ABattleGameMode::AreEnemiesEnabled() const
{
	return bEnemiesEnabled;
}

void ABattleGameMode::CacheSpawnPoints()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), SpawnPoints);

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn points found!"));
	}
	for (AActor* SpawnPoint : SpawnPoints)
	{
		SpawnPointBlockedUntil.Add(SpawnPoint, 0.0f);
	}
}

void ABattleGameMode::StartPhase()
{
	CurrentPhaseData = GeneratePhaseData(CurrentPhaseIndex);

	if (CurrentPhaseData.EnemyTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No enemy types for phase %d"), CurrentPhaseIndex);
		return;
	}

	EnemiesAlive = 0;

	UE_LOG(LogTemp, Warning, TEXT("Starting Phase %d with %d enemies"),
		CurrentPhaseIndex,
		CurrentPhaseData.NumberOfEnemies);
	if (ScreenMessageWidget)
	{
		FString PhaseText = FString::Printf(TEXT("PHASE %d"), CurrentPhaseIndex);
		ScreenMessageWidget->SetMessageText(PhaseText);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);

		GetWorldTimerManager().ClearTimer(PhaseMessageTimerHandle);
		GetWorldTimerManager().SetTimer(	PhaseMessageTimerHandle,	this,	&ABattleGameMode::HidePhaseMessage,	PhaseMessageDuration,	false);
	}
	for (int32 i = 0; i < CurrentPhaseData.NumberOfEnemies; i++)
	{
		SpawnEnemy();
	}
}

void ABattleGameMode::SpawnEnemy()
{
	const FPhaseData& Phase = CurrentPhaseData;
	
	UE_LOG(LogTemp, Warning, TEXT("EnemyTypes: %d | SpawnPoints: %d"), Phase.EnemyTypes.Num(), SpawnPoints.Num());

	if (Phase.EnemyTypes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("NO ENEMY TYPES SET!"));
		return;
	}

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("NO SPAWN POINTS FOUND!"));
		return;
	}
	int32 EnemyIndex = FMath::RandRange(0, Phase.EnemyTypes.Num() - 1);
	

	TSubclassOf<ABasePawn> EnemyClass = Phase.EnemyTypes[EnemyIndex];
	
	AActor* SpawnPoint = GetAvailableSpawnPoint();

	if (!SpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available spawn point right now"));
		return;
	}
	FVector Location = SpawnPoint->GetActorLocation();
	//Location.Z += 100.f;
	FRotator Rotation = SpawnPoint->GetActorRotation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABasePawn* SpawnedEnemy = GetWorld()->SpawnActor<ABasePawn>(EnemyClass, Location, Rotation, Params);

	if (SpawnedEnemy)
	{
		EnemiesAlive++;
		SpawnPointBlockedUntil.FindOrAdd(SpawnPoint) = GetWorld()->GetTimeSeconds() + SameSpawnPointCooldown;
		if (ATower* Tower = Cast<ATower>(SpawnedEnemy))
		{
			Tower->Tank = Tank;
		}
		else if (ARobotEnemy* Robot = Cast<ARobotEnemy>(SpawnedEnemy))
		{
			Robot->Tank = Tank;
		}
		else if (AExploderRobot* Exploder = Cast<AExploderRobot>(SpawnedEnemy))
		{
			Exploder->Tank = Tank;
		}
	}
}

void ABattleGameMode::StartNextPhase()
{
	CurrentPhaseIndex++;


	UE_LOG(LogTemp, Warning, TEXT("Next Phase starting... Phase %d"), CurrentPhaseIndex);

	FTimerHandle TimerHandle;GetWorldTimerManager().SetTimer(TimerHandle,this,&ABattleGameMode::StartPhase,TimeBetweenPhases,false);
}


AActor* ABattleGameMode::GetAvailableSpawnPoint()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	TArray<AActor*> AvailableSpawnPoints;

	for (AActor* SpawnPoint : SpawnPoints)
	{
		if (!SpawnPoint)
		{
			continue;
		}

		float* BlockedUntil = SpawnPointBlockedUntil.Find(SpawnPoint);

		if (!BlockedUntil || CurrentTime >= *BlockedUntil)
		{
			AvailableSpawnPoints.Add(SpawnPoint);
		}
	}

	if (AvailableSpawnPoints.Num() == 0)
	{
		return nullptr;
	}

	int32 SpawnIndex = FMath::RandRange(0, AvailableSpawnPoints.Num() - 1);
	return AvailableSpawnPoints[SpawnIndex];
}


FPhaseData ABattleGameMode::GeneratePhaseData(int32 PhaseNumber)
{
	FPhaseData PhaseData;

	// number of enemies grows with phase
	PhaseData.NumberOfEnemies = 3 + PhaseNumber;

	// every 5 phases → harder spike
	if (PhaseNumber % 5 == 0)
	{
		PhaseData.NumberOfEnemies += 2;
	}

	// always basic enemy
	if (BasicEnemyClass)
	{
		PhaseData.EnemyTypes.Add(BasicEnemyClass);
	}

	// unlock tower
	if (PhaseNumber >= 3 && HeavyEnemyClass)
	{
		PhaseData.EnemyTypes.Add(HeavyEnemyClass);
	}

	// unlock exploder
	if (PhaseNumber >= 6 && ExploderEnemyClass )
	{
		PhaseData.EnemyTypes.Add(ExploderEnemyClass);
		
	}
	if (PhaseNumber >= 10 && FastEnemyClass)
	{
		PhaseData.EnemyTypes.Add(FastEnemyClass);

	}

	return PhaseData;
}


void ABattleGameMode::HidePhaseMessage()
{
	if (ScreenMessageWidget)
	{
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABattleGameMode::OpenShop()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	if (!ShopWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopWidgetClass is null"));
		return;
	}

	if (ShopWidget)
	{
		ShopWidget->RemoveFromParent();
		ShopWidget = nullptr;
	}

	ShopWidget = CreateWidget<UUserWidget>(PC, ShopWidgetClass);

	if (ShopWidget)
	{
		ShopWidget->AddToViewport();
		PC->SetInputMode(FInputModeUIOnly());
		PC->bShowMouseCursor = true;

		UE_LOG(LogTemp, Warning, TEXT("SHOP OPENED"));
	}
}

bool ABattleGameMode::SpendCoins(int32 Amount)

{
	if (Coins < Amount)
	{
		return false;
	}

	Coins -= Amount;

	UE_LOG(LogTemp, Warning, TEXT("Coins spent: %d | Total Coins: %d"), Amount, Coins);

	if (Tank)
	{
		Tank->UpdateCoins(Coins);
	}

	return true;
}

void ABattleGameMode::CloseShopAndStartNextPhase()
{
	if (ShopWidget)
	{
		ShopWidget->RemoveFromParent();
		ShopWidget = nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	if (Tank)
	{
		Tank->ResetBurstState();
		Tank->SetPlayerEnabled(true);
		Tank->UpdateCoins(Coins);
	}

	bEnemiesEnabled = true;

	StartNextPhase();

	UE_LOG(LogTemp, Warning, TEXT("SHOP CLOSED -> NEXT PHASE"));
}


int32 ABattleGameMode::GetHealthUpgradeCost() const
{
	if (!Tank)
	{
		return HealthBaseCost;
	}

	return HealthBaseCost + (Tank->HealthLevel * HealthCostIncrease);
}

int32 ABattleGameMode::GetDamageUpgradeCost() const
{
	if (!Tank)
	{
		return DamageBaseCost;
	}

	return DamageBaseCost + (Tank->DamageLevel * DamageCostIncrease);
}

int32 ABattleGameMode::GetSpeedUpgradeCost() const
{
	if (!Tank)
	{
		return SpeedBaseCost;
	}

	return SpeedBaseCost + (Tank->SpeedLevel * SpeedCostIncrease);
}

int32 ABattleGameMode::GetBurstUpgradeCost() const
{
	if (!Tank)
	{
		return BurstBaseCost;
	}

	return BurstBaseCost + (Tank->ProjectileBurstLevel * BurstCostIncrease);
}


void ABattleGameMode::StartGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	RunStartTime = GetWorld()->GetTimeSeconds();
	PhasesSurvived = 0;
	if (StartMenuWidget)
	{
		StartMenuWidget->RemoveFromParent();
		StartMenuWidget = nullptr;
	}

	if (Tank)
	{
		Tank->CreateHUDIfNeeded();
		Tank->ShowHUD(true);
		Tank->SetPlayerEnabled(false);
	}

	if (PlayerController)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor =true;
	}
	CountdownSeconds = CountdownDelay;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&ABattleGameMode::OnCountdownTimerTimeout,1.0f,true);
}

void ABattleGameMode::ShowDeathMenu()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (DeathMenuWidgetClass && PlayerController)
	{
		DeathMenuWidget = CreateWidget<UUserWidget>(PlayerController, DeathMenuWidgetClass);
		if (DeathMenuWidget)
		{
			DeathMenuWidget->AddToViewport();

			FInputModeUIOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;
		}
	}
}



void ABattleGameMode::OnStartCountdownFinished()
{
	if (Tank)
	{
		Tank->SetPlayerEnabled(true);
	}
}


void ABattleGameMode::OnPlayerDied()
{
	// show your old defeat message here
	if (ScreenMessageWidget)
	{
		ScreenMessageWidget->SetMessageText(TEXT("DEFEAT"));
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
	}

	FTimerHandle DeathMenuTimerHandle;
	GetWorldTimerManager().SetTimer(DeathMenuTimerHandle,this,&ABattleGameMode::ShowDeathMenu,	2.0f, false);
}


	void ABattleGameMode::LoadHighScore()
	{
		if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
		{
			USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex);
			UBattleSaveGame* SaveGame = Cast<UBattleSaveGame>(LoadedGame);

			if (SaveGame)
			{
				HighScorePhases = SaveGame->BestPhasesSurvived;
				HighScoreTime = SaveGame->BestSurvivalTime;
			}
		}
		else
		{
			UBattleSaveGame* SaveGame = Cast<UBattleSaveGame>(
				UGameplayStatics::CreateSaveGameObject(UBattleSaveGame::StaticClass())
			);

			if (SaveGame)
			{
				UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, UserIndex);
			}
		}
	}


	void ABattleGameMode::SaveHighScoreIfNeeded()
	{
		bool bNewHighScore = false;

		if (PhasesSurvived > HighScorePhases)
		{
			bNewHighScore = true;
		}
		else if (PhasesSurvived == HighScorePhases && SurvivalTime > HighScoreTime)
		{
			bNewHighScore = true;
		}

		if (!bNewHighScore)
		{
			return;
		}

		HighScorePhases = PhasesSurvived;
		HighScoreTime = SurvivalTime;

		UBattleSaveGame* SaveGame = Cast<UBattleSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UBattleSaveGame::StaticClass())
		);

		if (SaveGame)
		{
			SaveGame->BestPhasesSurvived = HighScorePhases;
			SaveGame->BestSurvivalTime = HighScoreTime;

			UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, UserIndex);
		}
	}
