// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AFPS_Weapon.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "Character/AFPS_Character.h"


AAFPS_Weapon::AAFPS_Weapon()
{
	// tick enable
	PrimaryActorTick.bCanEverTick = true;

	// weapon attached to player mesh which attached to camera comp, need this to get proper weapon socket position in tick func
	SetTickGroup(ETickingGroup::TG_PostUpdateWork);  

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RootComponent = MeshComp;

	// find default mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/FPSLaserGun/FPWeapon/Mesh/SK_FPGun.SK_FPGun"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetSkeletalMesh(MeshFinder.Object);
	}

	// weapon defaults
	MuzzleSocketName = "Muzzle";
	AttachSocketName = "GripPoint";

	Range = 1'000'00.f;  // 1000 m
	Damage = 10.f;
	StartFireDelay = 0.5f;
	FireRate = 0.1f;
	EnergyLevel = 100.f;
	EnergyRecoveryRate = 50.f;  // 2 sec recovery
	EnergyDrainPerShot = 5.f;   // 2 sec shooting

	DamageType = UDamageType::StaticClass();
	
	ShotLineTraceChannel = ECollisionChannel::ECC_Camera;

	HitTraceQueryParams.AddIgnoredActor(this);
	HitTraceQueryParams.bTraceComplex = true;
}

void AAFPS_Weapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentEnergyLevel = EnergyLevel;
	EnergyLevelTarget = EnergyLevel;
}

void AAFPS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnTickCalculateEnergyLevel(DeltaTime);

	#if WITH_EDITOR
	if (bDrawDebugWeapon)
	{
		DrawDebug(DeltaTime);
	}
	#endif // WITH_EDITOR

}

void AAFPS_Weapon::StartFire()
{
	bWantsToFire = true;

	if (CanStartShooting() && HasEnergyForSingleShot())
	{
		StartFire_Internal();
	}
}

void AAFPS_Weapon::StopFire()
{
	bWantsToFire = false;

	StopFire_Internal();
}

void AAFPS_Weapon::OnAttach(AAFPS_Character* InCharacterOwner)
{
	if (InCharacterOwner)
	{
		CharacterOwner = InCharacterOwner;
		SetOwner(InCharacterOwner);

		HitTraceQueryParams.AddIgnoredActor(InCharacterOwner);
	}
}

bool AAFPS_Weapon::CanStartShooting()
{
	float DeltaTime = GetWorld()->TimeSince(LastTimeWhenFiringStarts);
	return DeltaTime >= StartFireDelay;
}

bool AAFPS_Weapon::HasEnergyForSingleShot()
{
	return CurrentEnergyLevel >= EnergyDrainPerShot;
}

void AAFPS_Weapon::StartFire_Internal()
{
	if (CharacterOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAFPS_Weapon::StartFire_Internal() CharacterOwner == nullptr"));
		return;
	}

	LastTimeWhenFiringStarts = GetWorld()->GetTimeSeconds();
	bIsFiring = true;

	CharacterOwner->PlayFireAnimMontage();  // character fire anim
	PlayStartFireEffects(); // effects

	FireLoop();
}

void AAFPS_Weapon::ResetShotTimer(bool bShouldReset)
{
	if (bShouldReset)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_FireLoop, this, &AAFPS_Weapon::FireLoop, FireRate);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_FireLoop);
	}
}

void AAFPS_Weapon::ShotLineTrace()
{
	if (CharacterOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAFPS_Weapon::ShotLineTrace() CharacterOwner is nullptr"));
		PLATFORM_BREAK();
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	CharacterOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();
	FVector ShotEnd = EyeLocation + ShotDirection * Range;

	LastHit = FHitResult();  // flush old result

	if (GetWorld()->LineTraceSingleByChannel(LastHit, EyeLocation, ShotEnd, ShotLineTraceChannel, HitTraceQueryParams))
	{
		AActor* HitActor = LastHit.GetActor();

		UGameplayStatics::ApplyPointDamage(HitActor, Damage, ShotDirection, LastHit, CharacterOwner->GetInstigatorController(), this, DamageType);

		PlayHitEffects();  // effects
	}
}

void AAFPS_Weapon::FireLoop()
{
	EnergyLevelTarget = CurrentEnergyLevel - EnergyDrainPerShot;

	ShotLineTrace();

	PlayFireEffects(); // effects

	if (HasEnergyForSingleShot())
	{
		ResetShotTimer(true);  // prepare next shot
	}
	else
	{
		bEnergyWasDrained = true;
		PlayEnergyDrainedEffects(); // effects

		StopFire_Internal();
	}
}

void AAFPS_Weapon::StopFire_Internal()
{
	EnergyLevelTarget = EnergyLevel;

	ResetShotTimer(false); // stop shooting loop
	bIsFiring = false;

	PlayEndFireEffects(); // effects
}

FORCEINLINE void AAFPS_Weapon::OnTickCalculateEnergyLevel(float DeltaSeconds)
{
	float InterpSpeed = bIsFiring ? (EnergyDrainPerShot / FireRate) : EnergyRecoveryRate;

	CurrentEnergyLevel = FMath::FInterpConstantTo(CurrentEnergyLevel, EnergyLevelTarget, DeltaSeconds, InterpSpeed);

	if (bEnergyWasDrained && CurrentEnergyLevel == EnergyLevel)
	{
		bEnergyWasDrained = false;
		PlayEnergyRestoredEffects();  // effects

		if (bWantsToFire) StartFire_Internal();  // run fire loop
	}
}

void AAFPS_Weapon::PlayStartFireEffects()
{
	OnPlayStartFireEffects(); // Calling blueprint version
}

void AAFPS_Weapon::PlayEndFireEffects()
{
	OnPlayEndFireEffects(); // Calling blueprint version
}

void AAFPS_Weapon::PlayFireEffects()
{
	OnPlayFireEffects(); // Calling blueprint version
}

void AAFPS_Weapon::PlayHitEffects()
{
	OnPlayHitEffects(); // Calling blueprint verison
}

void AAFPS_Weapon::PlayEnergyDrainedEffects()
{
	OnPlayEnergyDrainedEffects(); // Calling blueprint version
}

void AAFPS_Weapon::PlayEnergyRestoredEffects()
{
	OnPlayEnergyRestoredEffects(); // Calling blueprint version
}

FORCEINLINE void AAFPS_Weapon::DrawDebug(float DeltaSeconds)
{
	FVector DrawLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	DrawDebugSphere(GetWorld(), DrawLocation, 3.f, 4, FColor::Yellow, false, -1.f, 1);

	// dbg shooting time msg
	FString ShootTimeMsg;
	{
		static float ShootingTimer;
		static float NotShootingTimer;
		
		if (bIsFiring)
		{
			ShootingTimer += DeltaSeconds;
			NotShootingTimer = 0.f;
			ShootTimeMsg = "\nShootingTime: " + FString::SanitizeFloat(ShootingTimer);
		}
		else
		{
			NotShootingTimer += DeltaSeconds;
			ShootingTimer = 0.f;
			ShootTimeMsg = "\nNotShootingTime: " + FString::SanitizeFloat(NotShootingTimer);
		}
	}

	FString Msg = "bWantsToFire: " + FString(bWantsToFire ? "true" : "false") + 
		"\nbEnergyWasDrained: " + FString(bEnergyWasDrained ? "true" : "false") +
		"\nbIsFiring: " + FString((bIsFiring ? "true" : "false")) +
		"\nEnergyCurrent: " + FString::SanitizeFloat(CurrentEnergyLevel) + 
		"\nEnergyTarget:" + FString::SanitizeFloat(EnergyLevelTarget) +
		"\nEnergyInterpSpeed:" + FString::SanitizeFloat(bIsFiring ? (EnergyDrainPerShot / FireRate) : EnergyRecoveryRate)
		+ ShootTimeMsg;

	DrawDebugString(GetWorld(), DrawLocation, Msg, 0, FColor::Cyan, 0.f, true);

	// draw debug last hit
	if (LastHit.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), LastHit.Location, 10.f, 4, FColor::Yellow, false, -1.f, 0, 1.f);
	}
}
