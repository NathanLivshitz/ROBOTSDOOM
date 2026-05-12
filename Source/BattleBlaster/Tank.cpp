#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "TimerManager.h"
#include "HealthComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "GameHUDWidget.h"
#include "Projectile.h"

ATank::ATank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	//shield sphiere
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(CapsuleComp);
	ShieldMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ShieldMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldMesh->SetVisibility(false);
	ShieldMesh->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));
	CurrentProjectileDamage = NormalProjectileDamage;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	//widget display
	CreateHUDIfNeeded();
	ShowHUD(false);
	UpdateProjectileUI();
		
	
	UpdateHealthUI();
	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)

	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
	//SetPlayerEnabled(false);
	SetPlayerEnabled(true);
	EnableMobileInput();
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerController)
	{
		if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			FHitResult HitResult;
			PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
			RotateTurret(HitResult.ImpactPoint);
		}
		else
		{
			// Mobile fallback → aim forward
			FVector Forward = GetActorForwardVector();
			RotateTurret(GetActorLocation() + Forward * 1000.f);
		}
	}

		if (bHasAimPoint)
		{
			float CurrentTime = GetWorld()->GetTimeSeconds();

			if (CurrentTime - LastAimTime <= AimLineTimeout)
			{
				RotateTurret(LastAimPoint);

				if (ProjectileSpawnRoot)
				{
					DrawDottedAimLine(
						ProjectileSpawnRoot->GetComponentLocation(),
						LastAimPoint
					);
				}
			}
			else
			{
				bHasAimPoint = false;
			}
		}

		VerticalVelocity += Gravity * DeltaTime;

		FHitResult Hit;
		AddActorWorldOffset(
			FVector(0.f, 0.f, VerticalVelocity * DeltaTime),
			true,
			&Hit
		);

		if (Hit.IsValidBlockingHit())
		{
			VerticalVelocity = 0.f;
		}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::MoveInput);
		EIC->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::TurnInput);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::FirePressed);
		EIC->BindAction(ScrollProjectileAction, ETriggerEvent::Triggered, this, &ATank::ScrollProjectile);
	}
}

void ATank::MoveInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	FVector DeltaLocation = FVector(0.0f, 0.0f, 0.0f);
	DeltaLocation.X = CurrentMoveSpeed * InputValue * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	AddActorLocalOffset(DeltaLocation, true);

	
	//	FVector2D InputVector = Value.Get<FVector2D>();

	//	// Ignore tiny joystick values
	//	if (InputVector.SizeSquared() < 0.01f)
	//	{
	//		return;
	//	}

	//	// Move tank
	//	FVector Forward = GetActorForwardVector();
	//	FVector Right = GetActorRightVector();

	//	FVector MoveDirection =
	//		(Forward * InputVector.Y + Right * InputVector.X).GetSafeNormal();

	//	FVector Move =
	//		MoveDirection * CurrentMoveSpeed * GetWorld()->GetDeltaSeconds();

	//	AddActorWorldOffset(Move, true);

	//	// Rotate tank body to movement direction
	//	FRotator TargetRotation = MoveDirection.Rotation();
	//	FRotator NewRotation = FMath::RInterpTo(
	//		GetActorRotation(),
	//		TargetRotation,
	//		GetWorld()->GetDeltaSeconds(),
	//		2.0f      // rotation speed
	//	);

	//	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
	}


void ATank::TurnInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	FRotator DeltaRotation = FRotator(0.0f, 0.0f, 0.0f);
	DeltaRotation.Yaw = TurnRate * InputValue * GetWorld()->GetDeltaSeconds();

	AddActorLocalRotation(DeltaRotation, true);
}
void ATank::HandleDestruction()
{
	Super::HandleDestruction();

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetPlayerEnabled(false);

	IsAlive = false;

	ABattleGameMode* BattleGameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
	if (BattleGameMode)
	{
		BattleGameMode->OnPlayerDied();
	}
}

void ATank::SetPlayerEnabled(bool Enabled)
{
	if (PlayerController)
	{
		if (Enabled)
		{
			EnableInput(PlayerController);
		}
		else
		{
			DisableInput(PlayerController);
		}
	}
	//APlayerController* PC = Cast<APlayerController>(GetController());

	//if (!PC)
	//{
	//	PC = UGameplayStatics::GetPlayerController(this, 0);
	//}

	//if (!PC)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("SetPlayerEnabled failed: PlayerController is null"));
	//	return;
	//}

	//PlayerController = PC;

	//if (Enabled)
	//{
	//	EnableInput(PC);
	//}
	//else
	//{
	//	DisableInput(PC);
	//}
}
void ATank::ActivateSpeedBoost(float NewSpeed, float Duration)
{
	CurrentMoveSpeed = NewSpeed;

	GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);
	GetWorldTimerManager().SetTimer(SpeedBoostTimerHandle,this,&ATank::ResetMoveSpeed,Duration,false);
}

void ATank::ResetMoveSpeed()
{
	CurrentMoveSpeed = Speed;
}

float ATank::GetCurrentMoveSpeed() const
{
	return CurrentMoveSpeed;
}

void ATank::ActivateShieldBoost(float Duration)
{
	
	bShieldActive = true;
	ShieldMesh->SetWorldLocation(GetActorLocation());
	ShieldMesh->AttachToComponent(
		CapsuleComp,
		FAttachmentTransformRules::KeepWorldTransform
	);

	ShieldMesh->SetRelativeLocation(FVector::ZeroVector);
	ShieldMesh->SetRelativeRotation(FRotator::ZeroRotator);
	ShieldMesh->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));
	ShieldMesh->SetVisibility(true);

	GetWorldTimerManager().ClearTimer(ShieldTimerHandle);
	GetWorldTimerManager().SetTimer(ShieldTimerHandle,this,&ATank::ResetShield,Duration,false);
}

void ATank::ResetShield()
{
	bShieldActive = false;
	ShieldMesh->SetVisibility(false);
}

bool ATank::IsShieldActive() const
{
	return bShieldActive;
}

UHealthComponent* ATank::GetHealthComponent() const
{
	return FindComponentByClass<UHealthComponent>();
}


//damage projectile change

void ATank::ActivateDamageBoost(float NewDamage, float Duration)
{
	CurrentProjectileDamage = NewDamage;

	UE_LOG(LogTemp, Warning, TEXT("Damage boost activated! New damage: %f"), CurrentProjectileDamage);

	GetWorldTimerManager().ClearTimer(DamageBoostTimerHandle);
	GetWorldTimerManager().SetTimer(DamageBoostTimerHandle,this,&ATank::ResetProjectileDamage,Duration,false);
}

void ATank::ResetProjectileDamage()
{
	CurrentProjectileDamage = NormalProjectileDamage;
	UE_LOG(LogTemp, Warning, TEXT("Damage boost ended. Damage reset to: %f"), CurrentProjectileDamage);
}

float ATank::GetCurrentProjectileDamage() const
{
	return CurrentProjectileDamage;
}

void ATank::UpdateCoins(int32 Coins)
{
	if (HUDWidget)
	{
		HUDWidget->SetCoins(Coins);
	}
}


//shop upgrades

void ATank::UpgradeHealth(float Amount)
{
	MaxHealthValue += Amount;
	HealthLevel++;

	UHealthComponent* HC = GetHealthComponent();
	if (HC)
	{
		HC->SetMaxHealth(MaxHealthValue);
		HC->Heal(Amount);
	}
	UpdateHealthUI();
}

void ATank::UpgradeDamage()
{
	CurrentProjectileDamage += DamageUpgradeStep;
	DamageLevel++;
}

void ATank::UpgradeSpeed()
{
	CurrentMoveSpeed += SpeedUpgradeStep;
	SpeedLevel++;
}

void ATank::UpgradeProjectileCount()
{
	if (ProjectilesPerPress < MaxProjectilesPerPress)
	{
		ProjectilesPerPress++;
		ProjectileBurstLevel++;
	}
}


void ATank::BuySpecialProjectile()
{
	if (!bHasSpecialProjectile)
	{
		bHasSpecialProjectile = true;
	}
}



float ATank::GetMaxHealthValue() const
{
	return MaxHealthValue;
}

bool ATank::HasSpecialProjectile() const
{
	return bHasSpecialProjectile;
}

void ATank::ResetFire()
{
	bCanFire = true;
}

void ATank::FirePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("CurrentProjectileType: %d"), (int32)CurrentProjectileType);
	UE_LOG(LogTemp, Warning, TEXT("ExplosionAmmoBars: %d | IceAmmoBars: %d | ControlAmmoBars: %d"), ExplosionAmmoBars, IceAmmoBars, ControlAmmoBars);
	UE_LOG(LogTemp, Warning, TEXT("Unlocked -> Explosion: %d Ice: %d Control: %d"), bExplosionUnlocked, bIceUnlocked, bControlUnlocked);
	if (!bCanFire)
	{
		return;
	}

	bCanFire = false;

	ShotsRemainingInBurst = ProjectilesPerPress;

	if (CurrentProjectileType != EProjectileType::Normal && !HasAmmoForCurrentProjectile())
	{
		CurrentProjectileType = EProjectileType::Normal;
	}

	switch (CurrentProjectileType)
	{
	case EProjectileType::ExplosionMark:
		if (ExplosionProjectileClass)
		{
			ProjectileClass = ExplosionProjectileClass;
		}
		else if (NormalProjectileClass)
		{
			ProjectileClass = NormalProjectileClass;
		}
		break;

	case EProjectileType::Ice:
		if (IceProjectileClass)
		{
			ProjectileClass = IceProjectileClass;
		}
		else if (NormalProjectileClass)
		{
			ProjectileClass = NormalProjectileClass;
		}
		break;

	case EProjectileType::Control:
		if (ControlProjectileClass)
		{
			ProjectileClass = ControlProjectileClass;
		}
		else if (NormalProjectileClass)
		{
			ProjectileClass = NormalProjectileClass;
		}
		break;

	case EProjectileType::Normal:
	default:
		if (NormalProjectileClass)
		{
			ProjectileClass = NormalProjectileClass;
		}
		break;
	}

	FireBurstStep();
}

void ATank::FireBurstStep()
{
	if (ShotsRemainingInBurst <= 0)
	{
		bCanFire = true;
		return;
	}

	SpawnSingleProjectile();

	ShotsRemainingInBurst--;

	if (ShotsRemainingInBurst > 0)
	{
		GetWorldTimerManager().SetTimer(BurstFireTimerHandle,this,&ATank::FireBurstStep,BurstDelay,false);
	}
	else
	{
		bCanFire = true;
	}
}

void ATank::SpawnSingleProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s ProjectileClass is NULL"), *GetName());
		return;
	}

	if (!ProjectileSpawnRoot)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s ProjectileSpawnRoot is NULL"), *GetName());
		return;
	}

	if (CurrentProjectileType != EProjectileType::Normal && !HasAmmoForCurrentProjectile())
	{
		CurrentProjectileType = EProjectileType::Normal;
	}

	TArray<USceneComponent*> SpawnPoints;
	ProjectileSpawnRoot->GetChildrenComponents(false, SpawnPoints);

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no spawn points under ProjectileSpawnRoot"), *GetName());
		return;
	}

	USceneComponent* SpawnPoint = SpawnPoints[0];
	if (!SpawnPoint)
	{
		return;
	}

	FVector SpawnLocation = SpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,SpawnLocation,SpawnRotation,	SpawnParams);

	if (Projectile)
	{
		Projectile->SetOwner(this);
		Projectile->SetDamage(GetCurrentProjectileDamage());
		Projectile->SetProjectileType(GetCurrentProjectileType());
		if (GetCurrentProjectileType() != EProjectileType::Normal)
		{
			ConsumeAmmoForCurrentProjectile();
		}
	}
}


void ATank::ResetBurstState()
{
	GetWorldTimerManager().ClearTimer(BurstFireTimerHandle);
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);

	ShotsRemainingInBurst = 0;
	bCanFire = true;
}


void ATank::UpdateHealthUI()
{
	if (!HUDWidget)
	{
		return;
	}

	UHealthComponent* HC = GetHealthComponent();
	if (!HC)
	{
		return;
	}

	float CurrentHealth = HC->GetHealth();
	float MaxHealth = HC->GetMaxHealth();

	float Percent = 0.f;
	if (MaxHealth > 0.f)
	{
		Percent = CurrentHealth / MaxHealth;
	}

	HUDWidget->SetHealthPercent(Percent);

	FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
	HUDWidget->SetHealthText(HealthString);
}


void ATank::BuySpecialProjectileType(EProjectileType NewType)
{
	bHasSpecialProjectile = true;
	CurrentProjectileType = NewType;
	UpdateProjectileUI();
}


void ATank::UnlockExplosionProjectile()
{
	bExplosionUnlocked = true;
	UpdateProjectileUI();
}

void ATank::UnlockIceProjectile()
{
	bIceUnlocked = true;
	UpdateProjectileUI();
}

void ATank::UnlockControlProjectile()
{
	bControlUnlocked = true;
	UpdateProjectileUI();
	UE_LOG(LogTemp, Warning, TEXT("Control unlocked = %d"), bControlUnlocked);
}

void ATank::BuyExplosionAmmoBar()
{
	if (ExplosionAmmoBars < MaxAmmoBarsPerType)
	{
		ExplosionAmmoBars++;
		UpdateProjectileUI();
	}
}

void ATank::BuyIceAmmoBar()
{
	if (IceAmmoBars < MaxAmmoBarsPerType)
	{
		IceAmmoBars++;
		UpdateProjectileUI();
	}
}

void ATank::BuyControlAmmoBar()
{
	if (ControlAmmoBars < MaxAmmoBarsPerType)
	{
		ControlAmmoBars++;
		UpdateProjectileUI();
	}
	UE_LOG(LogTemp, Warning, TEXT("Control unlocked = %d"), bControlUnlocked);
}


bool ATank::HasAmmoForCurrentProjectile() const
{
	switch (CurrentProjectileType)
	{
	case EProjectileType::ExplosionMark:
		return bExplosionUnlocked && ExplosionAmmoBars > 0;

	case EProjectileType::Ice:
		return bIceUnlocked && IceAmmoBars > 0;

	case EProjectileType::Control:
		return bControlUnlocked && ControlAmmoBars > 0;

	case EProjectileType::Normal:
	default:
		return true;
	}
}

void ATank::ConsumeAmmoForCurrentProjectile()
{
	switch (CurrentProjectileType)
	{
	case EProjectileType::ExplosionMark:
		if (ExplosionAmmoBars > 0)
		{
			ExplosionAmmoBars--;
		}
		break;

	case EProjectileType::Ice:
		if (IceAmmoBars > 0)
		{
			IceAmmoBars--;
		}
		break;

	case EProjectileType::Control:
		if (ControlAmmoBars > 0)
		{
			ControlAmmoBars--;
		}
		break;

	default:
		break;
	}
	UpdateProjectileUI();
}
bool ATank::IsProjectileTypeSelectable(EProjectileType Type) const
{
	switch (Type)
	{
	case EProjectileType::Normal:
		return true;

	case EProjectileType::ExplosionMark:
		return bExplosionUnlocked && ExplosionAmmoBars > 0;

	case EProjectileType::Ice:
		return bIceUnlocked && IceAmmoBars > 0;

	case EProjectileType::Control:
		return bControlUnlocked && ControlAmmoBars > 0;

	default:
		return false;
	}
}

void ATank::ScrollProjectile(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();

	UE_LOG(LogTemp, Warning, TEXT("Scroll value: %f"), ScrollValue);

	if (ScrollValue > 0.0f)
	{
		NextProjectileType();
	}
	else if (ScrollValue < 0.0f)
	{
		PreviousProjectileType();
	}
}

void ATank::NextProjectileType()
{
	UE_LOG(LogTemp, Warning, TEXT("NextProjectileType called"));
	UE_LOG(LogTemp, Warning, TEXT("Current before scroll = %d"), (int32)CurrentProjectileType);
	TArray<EProjectileType> Types =
	{
		EProjectileType::Normal,
		EProjectileType::ExplosionMark,
		EProjectileType::Ice,
		EProjectileType::Control
	};

	int32 CurrentIndex = Types.IndexOfByKey(CurrentProjectileType);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentProjectileType = EProjectileType::Normal;
		return;
	}

	for (int32 i = 1; i <= Types.Num(); i++)
	{
		int32 NextIndex = (CurrentIndex + i) % Types.Num();
		if (IsProjectileTypeSelectable(Types[NextIndex]))
		{
			CurrentProjectileType = Types[NextIndex];
			UpdateProjectileUI();
			UE_LOG(LogTemp, Warning, TEXT("Projectile changed to %d"), (int32)CurrentProjectileType);
			return;
		}
	}
}

void ATank::PreviousProjectileType()
{
	UE_LOG(LogTemp, Warning, TEXT("PreviousProjectileType called"));
	UE_LOG(LogTemp, Warning, TEXT("Current before scroll = %d"), (int32)CurrentProjectileType);
	TArray<EProjectileType> Types =
	{
		EProjectileType::Normal,
		EProjectileType::ExplosionMark,
		EProjectileType::Ice,
		EProjectileType::Control
	};

	int32 CurrentIndex = Types.IndexOfByKey(CurrentProjectileType);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentProjectileType = EProjectileType::Normal;
		return;
	}

	for (int32 i = 1; i <= Types.Num(); i++)
	{
		int32 PrevIndex = (CurrentIndex - i + Types.Num()) % Types.Num();
		if (IsProjectileTypeSelectable(Types[PrevIndex]))
		{
			CurrentProjectileType = Types[PrevIndex];
			UpdateProjectileUI();
			UE_LOG(LogTemp, Warning, TEXT("Projectile changed to %d"), (int32)CurrentProjectileType);
			return;
		}
	}
}


int32 ATank::GetAmmoCountForType(EProjectileType Type) const
{
	switch (Type)
	{
	case EProjectileType::ExplosionMark:
		return ExplosionAmmoBars;

	case EProjectileType::Ice:
		return IceAmmoBars;

	case EProjectileType::Control:
		return ControlAmmoBars;

	case EProjectileType::Normal:
	default:
		return -1; // normal ammo = unlimited
	}
}

bool ATank::IsProjectileTypeSelected(EProjectileType Type) const
{
	return CurrentProjectileType == Type;
}

void ATank::UpdateProjectileUI()
{
	if (HUDWidget)
	{
		HUDWidget->UpdateProjectileDisplay(CurrentProjectileType,ExplosionAmmoBars,IceAmmoBars,	ControlAmmoBars);
	}
}

void ATank::CreateHUDIfNeeded()
{
	if (!HUDWidget && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UGameHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void ATank::ShowHUD(bool bShow)
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

void ATank::SelectProjectileType(EProjectileType Type)
{
	if (IsProjectileTypeSelectable(Type))
	{
		CurrentProjectileType = Type;
		UpdateProjectileUI();
	}
}


void ATank::EnableMobileInput()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	PC->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);
}


void ATank::MobileAimAtScreenPosition(FVector2D ScreenPosition)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	FVector WorldLocation;
	FVector WorldDirection;

	if (PC->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection))
	{
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = TraceStart + WorldDirection * 10000.f;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			LastAimPoint = Hit.ImpactPoint;
			//RotateTurret(Hit.ImpactPoint);
			bHasAimPoint = true;
			LastAimTime = GetWorld()->GetTimeSeconds();
			RotateTurret(LastAimPoint);
			DrawDottedAimLine(ProjectileSpawnRoot->GetComponentLocation(), Hit.ImpactPoint);
			
		}
	}
}



void ATank::DrawDottedAimLine(const FVector& Start, const FVector& End)
{
	const int32 NumDots = 10;

	for (int32 i = 0; i <= NumDots; i++)
	{
		float Alpha = (float)i / NumDots;
		FVector Point = FMath::Lerp(Start, End, Alpha);

		DrawDebugPoint(
			GetWorld(),
			Point,
			4.0f,          // dot size
			FColor::White,
			false,          // not persistent
			0.05f);           // duration
	}
}