#include "Projectile.h"

#include "Kismet/GameplayStatics.h"

#include "BasePawn.h"
// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SetRootComponent(ProjectileMesh);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.0f;
	ProjectileMovementComp->MaxSpeed = 1000.0f;

	TrailParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchSound, GetActorLocation());
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner)
	{
		Destroy();
		return;
	}

	if (OtherActor && OtherActor != MyOwner && OtherActor != this)
	{
		ABasePawn* HitPawn = Cast<ABasePawn>(OtherActor);
		switch (ProjectileType)
		{
		case EProjectileType::ExplosionMark:
		{
			
			if (HitPawn)
			{
				HitPawn->ApplyExplosionMark(MyOwner->GetInstigatorController());
			}
			break;
		}

		case EProjectileType::Ice:
		{
			
			if (HitPawn)
			{
				HitPawn->ApplyIceEffect();
			}
			break;
		}

		case EProjectileType::Control:
		{
			
			if (HitPawn)
			{
				HitPawn->ApplyControlEffect(MyOwner->GetInstigatorController());
			}
			break;
		}

		case EProjectileType::Normal:
		default:
		{
			UGameplayStatics::ApplyDamage(OtherActor,Damage,MyOwner->GetInstigatorController(),this,UDamageType::StaticClass());
			break;
		}
		}

		UE_LOG(LogTemp, Warning, TEXT("Projectile hit actor: %s"), *GetNameSafe(OtherActor));

		if (HitParticles)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitParticles, GetActorLocation(), GetActorRotation());
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}

		if (HitCameraShakeClass)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->ClientStartCameraShake(HitCameraShakeClass);
			}
		}
	}

	Destroy();
}

void AProjectile::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

float AProjectile::GetDamage() const
{
	return Damage;
}

void AProjectile::SetProjectileType(EProjectileType NewProjectileType)
{
	ProjectileType = NewProjectileType;
}

EProjectileType AProjectile::GetProjectileType() const
{
	return ProjectileType;
}