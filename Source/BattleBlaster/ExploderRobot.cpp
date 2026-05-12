// Fill out your copyright notice in the Description page of Project Settings.


#include "ExploderRobot.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "BattleGameMode.h"
AExploderRobot::AExploderRobot()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionTrigger"));
	ExplosionTrigger->SetupAttachment(RootComponent);
	ExplosionTrigger->SetSphereRadius(250.f);
	ExplosionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
void AExploderRobot::BeginPlay()
{
	Super::BeginPlay();
	if (ExplosionTrigger)
	{
		ExplosionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AExploderRobot::OnExplosionTriggerOverlap);
	}
}
void AExploderRobot::OnExplosionTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode || !GameMode->AreEnemiesEnabled())
	{
		
		return;
	}
	if (bHasExploded)
	{
		return;
	}
	if (!Tank)
	{
		
		return;
	}

	

	ATank* PlayerTank = Cast<ATank>(OtherActor);
	if (PlayerTank)
	{
		if (IsFrozen())
		{
			return;
		}

		Tank = PlayerTank;
		Explode();
	}
}
void AExploderRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AExploderRobot::Explode()
{
	if (bHasExploded)
	{
		return;
	}

	bHasExploded = true;
	bGiveCoinsOnDeath = false;

	if (Tank)
	{
		UGameplayStatics::ApplyDamage(Tank,ExplosionDamage,GetInstigatorController(),this,UDamageType::StaticClass());
	}

	if (ExplosionParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathParticles, GetActorLocation(), GetActorRotation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	
	ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->ActorDied(this);
	}
	else
	{
		Destroy();
	}
}
void AExploderRobot::CheckFireCondition()
{
	// Exploder robot does not shoot
}

