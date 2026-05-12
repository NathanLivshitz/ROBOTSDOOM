// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerBoost.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Tank.h"
// Sets default values
APowerBoost::APowerBoost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetSphereRadius(50.f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostMesh"));
	BoostMesh->SetupAttachment(RootComponent);
	BoostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void APowerBoost::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APowerBoost::OnOverlapBegin);
}

// Called every frame
void APowerBoost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	FVector NewLocation = StartLocation;
	NewLocation.Z += FMath::Sin(RunningTime * FloatSpeed) * FloatAmplitude;
	SetActorLocation(NewLocation);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotationSpeed * DeltaTime;
	SetActorRotation(NewRotation);
}

void APowerBoost::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	ATank* Tank = Cast<ATank>(OtherActor);
	if (Tank)
	{
		ApplyBoost(Tank);
		Destroy();
		UE_LOG(LogTemp, Warning, TEXT("PowerBoost overlapped by tank"));
	}
}

void APowerBoost::ApplyBoost(ATank* Tank)
{
}
