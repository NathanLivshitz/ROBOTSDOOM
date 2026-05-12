// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "RobotEnemy.generated.h"

/**
 * 
 */
class ATank;
class USkeletalMeshComponent;
UCLASS()
class BATTLEBLASTER_API ARobotEnemy : public ABasePawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* RobotMesh;
public:
	ARobotEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void HandleDestruction();
	ATank* Tank = nullptr;

	UFUNCTION(BlueprintCallable)
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable)
	bool IsShooting() const { return bIsShooting; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Effect")
	float ControlledTargetSearchRadius = 10000.f;
private:
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 800.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate = 2.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 300.f;

	

	UPROPERTY(EditAnywhere, Category = "AI")
	float DetectionRange = 2000.f;

	FTimerHandle FireTimerHandle;
	bool bIsMoving = false;
	bool bIsShooting = false;

	FTimerHandle ShootAnimationTimerHandle;

	void ResetShootingState();
	virtual void CheckFireCondition();
	void MoveToTank(float DeltaTime);
	float GetDistanceToTank() const;
	bool IsTankInDetectionRange() const;
	ABasePawn* GetControlledTarget() const;
	float GetDistanceToActor(AActor* OtherActor) const;
};
