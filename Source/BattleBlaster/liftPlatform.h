#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Tank.h"
#include "liftPlatform.generated.h"

UCLASS()
class BATTLEBLASTER_API AliftPlatform : public AActor
{
	GENERATED_BODY()

public:
	AliftPlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY()
	ATank* Tank = nullptr;

	FVector StartLocation;
	float RunningTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Lift")
	float Amplitude = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Lift")
	float Frequency = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Lift")
	float LiftHeight = 300.f;

	UPROPERTY(EditAnywhere, Category = "Lift")
	float LiftSpeed = 150.f;

	bool bGoingUp = true;

	FVector BottomLocation;
	FVector TopLocation;
	UFUNCTION()
	void OnTriggerBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnTriggerEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};