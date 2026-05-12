#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
//#include "Projectile.h"

#include "BasePawn.generated.h"

class AProjectile;
class USceneComponent;
UCLASS()
class BATTLEBLASTER_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TurretMesh;

	/*UPROPERTY(VisibleAnywhere)
	USceneComponent* ProjectileSpawnPoint;*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ProjectileSpawnRoot;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DeathParticles;

	UPROPERTY(EditAnywhere)
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Reward")
	int32 CoinReward = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reward")
	bool bGiveCoinsOnDeath = true;

	void RotateTurret(FVector LookAtTarget);

	UFUNCTION(BlueprintCallable)
	void Fire();

	virtual void HandleDestruction();

	int32 GetCoinReward() const { return CoinReward; }
	bool ShouldGiveCoins() const { return bGiveCoinsOnDeath; }



	// Explosion Mark
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Mark")
	bool bIsExplosionMarked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Mark")
	float MarkDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Mark")
	float MarkExplosionDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Mark")
	float MarkExplosionRadius = 300.0f;

	UPROPERTY()
	AController* MarkInstigatorController = nullptr;

	FTimerHandle MarkExplosionTimer;

public:

	UFUNCTION(BlueprintCallable)
	virtual void ApplyExplosionMark(AController* InstigatorController);

	UFUNCTION()
	virtual void ExplodeMarkedEnemy();

	UFUNCTION(BlueprintImplementableEvent)
	void SetMarkedVisuals();


	// Ice Effect
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ice Effect")
	bool bIsFrozen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ice Effect")
	float IceDuration = 10.0f;

	FTimerHandle IceTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ApplyIceEffect();

	UFUNCTION()
	virtual void ResetIceEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void SetFrozenVisuals();

	UFUNCTION(BlueprintImplementableEvent)
	void ClearFrozenVisuals();

	UFUNCTION(BlueprintCallable)
	bool IsFrozen() const { return bIsFrozen; }

	// Control Effect
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Effect")
	bool bIsControlled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Effect")
	float ControlDuration = 10.0f;

	FTimerHandle ControlTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ApplyControlEffect(AController* InstigatorController);

	UFUNCTION()
	virtual void ResetControlEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void SetControlledVisuals();

	UFUNCTION(BlueprintImplementableEvent)
	void ClearControlledVisuals();

	

	UFUNCTION(BlueprintCallable)
	bool IsEnemyControlled() const { return bIsControlled; }
};
