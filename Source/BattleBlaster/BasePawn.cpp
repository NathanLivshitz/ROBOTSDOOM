#include "BasePawn.h"

#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Projectile.h"
#include "GameFramework/DamageType.h"
// Sets default values
ABasePawn::ABasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	//ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	//ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	ProjectileSpawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnRoot"));
	ProjectileSpawnRoot->SetupAttachment(TurretMesh);

}

void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	FVector VectorToTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.0f, VectorToTarget.Rotation().Yaw, 0.0f);

	FRotator InterpolatedRotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(),LookAtRotation,GetWorld()->GetDeltaSeconds(),	10.0f);

	TurretMesh->SetWorldRotation(InterpolatedRotation);
}

void ABasePawn::Fire()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s ProjectileSpawnPoint is NULL"), *GetName());
		return;
	}

	TArray<USceneComponent*> SpawnPoints;
	ProjectileSpawnRoot->GetChildrenComponents(false, SpawnPoints);

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no spawn points under ProjectileSpawnRoot"), *GetName());
		return;
	}

	

	for (USceneComponent* SpawnPoint : SpawnPoints)
	{
		if (!SpawnPoint)
		{
			continue;
		}

		FVector SpawnLocation = SpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,SpawnLocation,SpawnRotation,SpawnParams);

		if (Projectile)
		{
			Projectile->SetOwner(this);

			ATank* Tank = Cast<ATank>(this);
			if (Tank)
			{
				Projectile->SetDamage(Tank->GetCurrentProjectileDamage());
				Projectile->SetProjectileType(Tank->GetCurrentProjectileType());
			}
		}
	}
}

void ABasePawn::HandleDestruction()
{
	if (DeathParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticles, GetActorLocation(), GetActorRotation());
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(DeathCameraShakeClass);
	}
}


void ABasePawn::ApplyExplosionMark(AController* InstigatorController)
{
	if (bIsExplosionMarked)
	{
		return;
	}

	bIsExplosionMarked = true;
	MarkInstigatorController = InstigatorController;

	// Blueprint event - make enemy glow red there
	SetMarkedVisuals();

	GetWorldTimerManager().SetTimer(MarkExplosionTimer,this,&ABasePawn::ExplodeMarkedEnemy,MarkDelay,false);
}

void ABasePawn::ExplodeMarkedEnemy()
{
	if (DeathParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),DeathParticles,GetActorLocation(),GetActorRotation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),GetActorLocation(),MarkExplosionRadius,ObjectTypes,ABasePawn::StaticClass(),ActorsToIgnore,	OutActors);

	for (AActor* HitActor : OutActors)
	{
		ABasePawn* HitPawn = Cast<ABasePawn>(HitActor);
		if (!HitPawn)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(HitPawn,MarkExplosionDamage,MarkInstigatorController,this,UDamageType::StaticClass());
	}

	// kill the marked pawn itself too
	UGameplayStatics::ApplyDamage(this,99999.f,MarkInstigatorController,this,UDamageType::StaticClass());
}


void ABasePawn::ApplyIceEffect()
{
	if (bIsFrozen)
	{
		return;
	}

	bIsFrozen = true;

	SetFrozenVisuals();

	GetWorldTimerManager().ClearTimer(IceTimerHandle);
	GetWorldTimerManager().SetTimer(IceTimerHandle,this,&ABasePawn::ResetIceEffect,IceDuration,false);
}

void ABasePawn::ResetIceEffect()
{
	bIsFrozen = false;
	ClearFrozenVisuals();
}

void ABasePawn::ApplyControlEffect(AController* InstigatorController)
{
	if (bIsControlled)
	{
		return;
	}

	bIsControlled = true;

	SetControlledVisuals();

	GetWorldTimerManager().ClearTimer(ControlTimerHandle);
	GetWorldTimerManager().SetTimer(ControlTimerHandle,this,&ABasePawn::ResetControlEffect,ControlDuration,false);
}

void ABasePawn::ResetControlEffect()
{
	bIsControlled = false;
	ClearControlledVisuals();
}