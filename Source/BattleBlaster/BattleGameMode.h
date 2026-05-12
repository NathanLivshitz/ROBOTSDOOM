// Fill out your copyright notice in the Description page of Project Settings.


#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Tank.h"
#include "ScreenMessage.h"
#include "BattleSaveGame.h"
#include "BattleGameMode.generated.h"

/**
 *
 */

 //phase struct 

USTRUCT(BlueprintType)
struct FPhaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ABasePawn>> EnemyTypes;
};


UCLASS()
class BATTLEBLASTER_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coins")//coints count
	int32 Coins = 0;
public:
	bool bEnemiesEnabled = false;

	bool AreEnemiesEnabled() const;

	ATank* Tank;

	void ActorDied(AActor* DeadActor);
	bool IsVictory = false;
	void OnGameOverTimerTimeout();

	UPROPERTY(EditAnywhere)
	float GameOverDelay = 3.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UScreenMessage> ScreenMessageClass;

	UScreenMessage* ScreenMessageWidget;

	UPROPERTY(EditAnywhere)
	int32 CountdownDelay = 3;

	int32 CountdownSeconds;
	bool IsGameOver = false;

	FTimerHandle CountdownTimerHandle;
	void OnCountdownTimerTimeout();
	void ShowPickupMessage(const FString& Message);
	//coins system
	void AddCoins(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetCoins() const;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TSubclassOf<ABasePawn> BasicEnemyClass;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TSubclassOf<ABasePawn> HeavyEnemyClass;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TSubclassOf<ABasePawn> ExploderEnemyClass;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TSubclassOf<ABasePawn> FastEnemyClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Phases")
	int32 CurrentPhaseIndex = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phases")
	int32 EnemiesAlive = 0;

	UPROPERTY(EditAnywhere, Category = "Phases")
	float TimeBetweenPhases = 3.0f;
	FPhaseData CurrentPhaseData;
	UPROPERTY()
	TArray<AActor*> SpawnPoints;
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SameSpawnPointCooldown = 5.0f;
	AActor* GetAvailableSpawnPoint();
	UPROPERTY()
	TMap<AActor*, float> SpawnPointBlockedUntil;
	void CacheSpawnPoints();
	void StartPhase();
	void StartNextPhase();
	void SpawnEnemy();
	FPhaseData GeneratePhaseData(int32 PhaseNumber);
	UPROPERTY(EditAnywhere, Category = "Phases")
	float PhaseMessageDuration = 2.0f;
	
	FTimerHandle PhaseMessageTimerHandle;

	void HidePhaseMessage();



	// ===== SHOP =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<UUserWidget> ShopWidgetClass;

	UPROPERTY()
	UUserWidget* ShopWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 HealthBaseCost = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 DamageBaseCost = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 SpeedBaseCost = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 BurstBaseCost = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 SpecialProjectileCostFire = 120;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 SpecialProjectileCostIce = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 SpecialProjectileCostPoison = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 HealthCostIncrease = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 DamageCostIncrease = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 SpeedCostIncrease = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 BurstCostIncrease = 30;


	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OpenShop();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseShopAndStartNextPhase();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetHealthUpgradeCost() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetDamageUpgradeCost() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetSpeedUpgradeCost() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	int32 GetBurstUpgradeCost() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool SpendCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void ShowDeathMenu();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> StartMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DeathMenuWidgetClass;

	UPROPERTY()
	UUserWidget* StartMenuWidget;

	UPROPERTY()
	UUserWidget* DeathMenuWidget;
	

	UFUNCTION(BlueprintCallable)
	void OnStartCountdownFinished();

	UFUNCTION()
	void OnPlayerDied();


	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 PhasesSurvived = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float RunStartTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float SurvivalTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "High Score")
	int32 HighScorePhases = 0;

	UPROPERTY(BlueprintReadOnly, Category = "High Score")
	float HighScoreTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "High Score")
	FString SaveSlotName = "BattleBlasterSave";

	UPROPERTY(EditAnywhere, Category = "High Score")
	int32 UserIndex = 0;

	void LoadHighScore();
	void SaveHighScoreIfNeeded();
};



