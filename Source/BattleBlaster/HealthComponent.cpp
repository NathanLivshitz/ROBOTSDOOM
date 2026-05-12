// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Health = MaxHealth;
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnDamageTaken);

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		BattleGameMode = Cast<ABattleGameMode>(GameMode);
	}

}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UHealthComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	//damage not taken if sphier is on 
	ATank* Tank = Cast<ATank>(GetOwner());
	if (Tank && Tank->IsShieldActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("Shield blocked damage"));
		return;
	}
	if (Damage > 0.0f)
	{
		Health -= Damage;
	
		if (Tank)
		{
			Tank->UpdateHealthUI();
		}
		/*UE_LOG(LogTemp, Warning, TEXT("Damage causer: %s"), *GetNameSafe(DamageCauser));
		UE_LOG(LogTemp, Warning, TEXT("OnDamageTaken | Owner=%s | DamagedActor=%s | Damage=%f | Causer=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(DamagedActor),
			Damage,
			*GetNameSafe(DamageCauser));*/
		if (Health <= 0.0f)
		{
			Health = 0.0f;
			if (BattleGameMode)
			{
				BattleGameMode->ActorDied(DamagedActor);
			}

		}
	}
}
//health to full boost
void UHealthComponent::HealToFull()
{
	Health = MaxHealth;
	ATank* Tank = Cast<ATank>(GetOwner());
	if (Tank)
	{
		Tank->UpdateHealthUI();
	}
	UE_LOG(LogTemp, Warning, TEXT("%s healed to full health"), *GetOwner()->GetName());
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
}

void UHealthComponent::Heal(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
}


float UHealthComponent::GetHealth() const
{
	return Health;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}