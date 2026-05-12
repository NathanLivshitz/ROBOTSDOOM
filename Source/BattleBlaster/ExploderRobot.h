// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RobotEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "ExploderRobot.generated.h"

/**
 * 
 */
class UParticleSystem;
class USoundBase;
class USphereComponent;
UCLASS()
class BATTLEBLASTER_API AExploderRobot : public ARobotEnemy
{
	GENERATED_BODY()
public:
	AExploderRobot();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ExplosionTrigger;

	UFUNCTION()
	void OnExplosionTriggerOverlap(UPrimitiveComponent* OverlappedComp,AActor*  OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
private:
	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplodeDistance = 250.f;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	float ExplosionDamage = 40.f;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	UNiagaraSystem* ExplosionParticles;

	UPROPERTY(EditAnywhere, Category = "Explosion")
	USoundBase* ExplosionSound;

	bool bHasExploded = false;
	void CheckFireCondition() override;
	void Explode();
};
