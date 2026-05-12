#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"

#include "InputAction.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Projectile.h"
#include "Tank.generated.h"

class UCameraComponent;
class UInputMappingContext;
class EnhancedInputSubsystems;
class UStaticMeshComponent;
class UHealthComponent;
class UGameHUDWidget;
class AProjectile;
class UFloatingPawnMovement;
/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ATank : public ABasePawn
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float NormalMoveSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float CurrentMoveSpeed = 500.f;

	FTimerHandle SpeedBoostTimerHandle;
	
	void ResetMoveSpeed();
	//shield 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	UStaticMeshComponent* ShieldMesh;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
	bool bShieldActive = false;

	FTimerHandle ShieldTimerHandle;

	void ResetShield();
	//damage
	UPROPERTY(EditAnywhere, Category = "Combat")
	float NormalProjectileDamage = 25.f;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CurrentProjectileDamage = 25.f;

	FTimerHandle DamageBoostTimerHandle;

	void ResetProjectileDamage();
	//widget
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UGameHUDWidget> HUDWidgetClass;

	UPROPERTY()
	UGameHUDWidget* HUDWidget;
public:
	ATank();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called to bind functionality to input
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere)
	float Speed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* TurnAction;
	UPROPERTY(EditAnywhere)
	float TurnRate = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ScrollProjectileAction;

	void ScrollProjectile(const FInputActionValue& Value);

	APlayerController* PlayerController;

	bool IsAlive = true;


	// ===== SHOP / UPGRADES =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	float MaxHealthValue = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	float DamageUpgradeStep = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	float SpeedUpgradeStep = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 ProjectilesPerPress = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 MaxProjectilesPerPress = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	float BurstDelay = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 ProjectileBurstLevel = 0;

	FTimerHandle BurstFireTimerHandle;

	int32 ShotsRemainingInBurst = 0;

	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<AProjectile> NormalProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<AProjectile> ExplosionProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<AProjectile> IceProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<AProjectile> ControlProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bHasSpecialProjectile = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 HealthLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 DamageLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 SpeedLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bCanFire = true;
	FTimerHandle FireRateTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpgradeHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpgradeDamage();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpgradeSpeed();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpgradeProjectileCount();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuySpecialProjectile();


	UFUNCTION(BlueprintCallable, Category = "Shop")
	float GetMaxHealthValue() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool HasSpecialProjectile() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void ResetFire();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void FirePressed();


	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyExplosionAmmoBar();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyIceAmmoBar();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyControlAmmoBar();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UnlockExplosionProjectile();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UnlockIceProjectile();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UnlockControlProjectile();

	void BuySpecialProjectileType(EProjectileType NewType);

	void FireBurstStep();
	void SpawnSingleProjectile();

	void MoveInput(const FInputActionValue& Value);
	void TurnInput(const FInputActionValue& Value);
	void HandleDestruction();
	void SetPlayerEnabled(bool Enabled);
	void ActivateSpeedBoost(float NewSpeed, float Duration);
	float GetCurrentMoveSpeed() const;
	void ActivateShieldBoost(float Duration);
	bool IsShieldActive() const; 
	UHealthComponent* GetHealthComponent() const;//health boost function 

	void ActivateDamageBoost(float NewDamage, float Duration);
	float GetCurrentProjectileDamage() const;
	void UpdateCoins(int32 Coins);


	UFUNCTION(BlueprintCallable)
	void ResetBurstState();

	UFUNCTION(BlueprintCallable)
	void UpdateHealthUI();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EProjectileType CurrentProjectileType = EProjectileType::Normal;


	EProjectileType GetCurrentProjectileType() const { return CurrentProjectileType; }
	void SetCurrentProjectileType(EProjectileType NewType) { CurrentProjectileType = NewType; }



	// unlocks
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	bool bExplosionUnlocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	bool bIceUnlocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	bool bControlUnlocked = false;

	// ammo bars per projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	int32 ExplosionAmmoBars = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	int32 IceAmmoBars = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	int32 ControlAmmoBars = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Ammo")
	int32 MaxAmmoBarsPerType = 5;


	bool HasAmmoForCurrentProjectile() const;
	void ConsumeAmmoForCurrentProjectile();


	//scrolling through projectile types
	UFUNCTION(BlueprintCallable, Category = "Special Ammo")
	void NextProjectileType();

	UFUNCTION(BlueprintCallable, Category = "Special Ammo")
	void PreviousProjectileType();

	UFUNCTION(BlueprintCallable, Category = "Special Ammo")
	void SelectProjectileType(EProjectileType Type);

	bool IsProjectileTypeSelectable(EProjectileType Type) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetAmmoCountForType(EProjectileType Type) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsProjectileTypeSelected(EProjectileType Type) const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateProjectileUI();

	UFUNCTION(BlueprintCallable)
	void CreateHUDIfNeeded();

	UFUNCTION(BlueprintCallable)
	void ShowHUD(bool bShow);
	void EnableMobileInput();

	UFUNCTION(BlueprintCallable, Category = "Mobile")
	void MobileAimAtScreenPosition(FVector2D ScreenPosition);

	void DrawDottedAimLine(const FVector& Start, const FVector& End);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobile Aim")
	FVector LastAimPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobile Aim")
	bool bHasAimPoint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobile Aim")
	float LastAimTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mobile Aim")
	float AimLineTimeout = 0.5f;

	float VerticalVelocity = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gravity")
	float Gravity = -1000.f;
};
