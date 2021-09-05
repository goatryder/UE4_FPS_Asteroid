// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Weapon.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "AFPS_Character.h"

// todo: fix energy level target 

AAFPS_Weapon::AAFPS_Weapon()
{
	// tick enable
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RootComponent = MeshComp;

	BeamPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BeamPSC"));
	BeamPSC->bAutoActivate = false;

	// find default mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/LaserGun/FPWeapon/Mesh/SK_FPGun.SK_FPGun"));
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

	CurrentEnergyLevel = EnergyLevel;
	EnergyLevelTarget = EnergyLevel;
}

void AAFPS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnTickCalculateEnergyLevel(DeltaTime);

	OnTickBeamPSCHandle();

	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		DrawDebug(DeltaTime);
	#endif

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

FORCEINLINE void AAFPS_Weapon::OnTickBeamPSCHandle()
{
	if (BeamPSC)
	{
		FVector MuzzleLoc = GetMesh()->GetSocketLocation(MuzzleSocketName);
		BeamPSC->SetBeamSourcePoint(0, MuzzleLoc, 0);
		
		FVector TargetLoc;
		{
			float BeamLength = LastHit.bBlockingHit ? LastHit.Distance : Range;
			FVector ShotMuzzleDirection;

			if (CharacterOwner)
			{
				ShotMuzzleDirection = (CharacterOwner->GetCharacterViewPoint() - MuzzleLoc);
			}
			else
			{
				ShotMuzzleDirection = (LastHit.TraceEnd - MuzzleLoc);
			}

			ShotMuzzleDirection.Normalize();
			TargetLoc = MuzzleLoc + ShotMuzzleDirection * BeamLength;
		}
		BeamPSC->SetBeamTargetPoint(0, TargetLoc, 0);

		// debug
		// DrawDebugLine(GetWorld(), MuzzleLoc, TargetLoc, FColor::Yellow, false, 0.f, 0, 2.f);
	}
}

void AAFPS_Weapon::PlayStartFireEffects()
{
	// beam particle
	if (BeamPSC)
	{
		BeamPSC->Activate();
	}

	OnPlayStartFireEffects(); // Calling blueprint version
}

void AAFPS_Weapon::PlayEndFireEffects()
{
	// beam particle
	if (BeamPSC)
	{
		BeamPSC->Deactivate();
	}

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
	// draw debug weapon parameters
	FVector DrawLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

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
		DrawDebugSphere(GetWorld(), LastHit.Location, 10.f, 4, FColor::Yellow);
	}
}
