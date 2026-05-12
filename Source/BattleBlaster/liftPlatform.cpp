#include "liftPlatform.h"

AliftPlatform::AliftPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	// Real platform mesh - blocks the tank
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	RootComponent = PlatformMesh;

	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);

	// Trigger box - only detects when tank is on platform
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetBoxExtent(FVector(250.f, 250.f, 30.f));
	TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AliftPlatform::OnTriggerBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AliftPlatform::OnTriggerEnd);
}

void AliftPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	BottomLocation = GetActorLocation();
	TopLocation = BottomLocation + FVector(0.f, 0.f, LiftHeight);
}

void AliftPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector TargetLocation = bGoingUp ? TopLocation : BottomLocation;

	FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, LiftSpeed);

	SetActorLocation(NewLocation, true);

	if (FVector::Dist(GetActorLocation(), TargetLocation) < 2.f)
	{
		bGoingUp = !bGoingUp;
	}
}

void AliftPlatform::OnTriggerBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	ATank* OverlappingTank = Cast<ATank>(OtherActor);

	if (OverlappingTank)
	{
		Tank = OverlappingTank;

		Tank->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		UE_LOG(LogTemp, Warning, TEXT("Tank attached to lift"));
	}
}

void AliftPlatform::OnTriggerEnd(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
	if (OtherActor == Tank)
	{
		Tank->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		Tank = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("Tank detached from lift"));
	}
}