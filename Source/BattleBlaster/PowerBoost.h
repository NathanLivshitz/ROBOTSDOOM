// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerBoost.generated.h"
class USphereComponent;
class UStaticMeshComponent;
class ATank;
UCLASS()
class BATTLEBLASTER_API APowerBoost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerBoost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoostMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost")
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float FloatAmplitude = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float FloatSpeed = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating")
	float RotationSpeed = 60.f;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	virtual void ApplyBoost(ATank* Tank) ;

private:
	FVector StartLocation;
	float RunningTime = 0.f;
};
