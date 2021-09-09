// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_AsteroidSpawner.h"

#include "AFPS_Asteroid.h"
#include "AFPS_GameMode.h"

#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif // WITH_EDITOR

AAFPS_AsteroidSpawner::AAFPS_AsteroidSpawner()
{
	#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	bDrawDebugAsteroidSpawn = true;
	#endif  // WITH_EDITOR

	// defaults
	SpawnParam.AsteroidClass = AAFPS_Asteroid::StaticClass();
	SpawnParam.AsteroidForwardVector = FVector::DownVector;  // to face asteroid ass to spawn anchor
	SpawnParam.InitialSpawnRadius = 25'00.f;  // 25m
	SpawnParam.MaxSpawnRadius = 1'000'00.f; // 1km
	SpawnParam.NextWaveRadiusMult = 1.05f;
	SpawnParam.InitialAsteroidSpawnNr = 15;
	SpawnParam.SpawnedAsteroidLimitMax = 1000;
	SpawnParam.NextWaveAsteroidSpawnNrMult = 1.1f;
	SpawnParam.NextWaveAsteroidScaleMult = 1.03f;
	SpawnParam.AsteroidScaleLimit = 3.f;
	SpawnParam.MinSpawnDistanceBetweenAsteroids = 1'00.f;  // 1m 
	SpawnParam.AsteroidKillNrToTriggerNextWave = 10;

	bAllowStartWave = true;  // allow execute initial spawn wave
}

bool AAFPS_AsteroidSpawner::CanSpawnWave()
{
	TArray<AActor*> InAsteroidActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnParam.AsteroidClass, InAsteroidActors);
	bool bAsteroidSpawnLimitReached = InAsteroidActors.Num() >= SpawnParam.SpawnedAsteroidLimitMax;

	return !bAsteroidSpawnLimitReached && bAllowStartWave;
}

void AAFPS_AsteroidSpawner::PrepareFirstWave(AAFPS_GameMode* GM)
{
	if (GM == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AsteroidSpawner::PrepareFirstWave(AAFPS_GameMode*) GM is nullptr!"));
		return;
	}

	if (CanSpawnWave())
	{
		// subscribe to asteroid kill for checking next spawn wave
		GM->NotifyAsteroidKilled.AddDynamic(this, &AAFPS_AsteroidSpawner::OnAsteroidKilled);

		// first wave spawn parameters
		WaveCount = 1;
		SpawnRadius = SpawnParam.InitialSpawnRadius;
		SpawnOrigin = GetNewSpawnOrigin();
		AsteroidToKillForNextWave = SpawnParam.AsteroidKillNrToTriggerNextWave;
		AsteroidSpawnNum = SpawnParam.InitialAsteroidSpawnNr;
		AsteroidScale = 1.f;

		// run first wave
		StartWave();
	}
}

void AAFPS_AsteroidSpawner::StartNextWave()
{
	if (CanSpawnWave())
	{
		// next wave spawn parameters
		++WaveCount;
		SpawnRadius = FMath::Min(SpawnRadius * SpawnParam.NextWaveRadiusMult, SpawnParam.MaxSpawnRadius);
		SpawnOrigin = GetNewSpawnOrigin();
		AsteroidToKillForNextWave = SpawnParam.AsteroidKillNrToTriggerNextWave;  // reset asteroid to kill nr
		AsteroidSpawnNum *= SpawnParam.NextWaveAsteroidSpawnNrMult;
		AsteroidScale = FMath::Min(AsteroidScale * SpawnParam.NextWaveAsteroidScaleMult, SpawnParam.AsteroidScaleLimit);
		
		// run next wave
		StartWave();
	}
}

void AAFPS_AsteroidSpawner::StartWave()
{
	bAllowStartWave = false;

	// debug
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Red, "Start Next wave");

	// spawn asteroids
	for (int It = 0; It != AsteroidSpawnNum; ++It)
	{
		SpawnAsteroid();
	}
}

FTransform AAFPS_AsteroidSpawner::CalcNextSpawnTransform()
{
	/*
	 * Spherical coordinates https://en.wikipedia.org/wiki/Spherical_coordinate_system
	 * 
	 * SpherePitch(Inclination) = arccos(z / sqrt(x^2+y^2+z^2)) = arccos(z/r) = arctan(sqrt(x^2 + y^2) / z)
	 * SphereYaw(Azimuth) = arctan(y/x)
	 * 
	 * x = r * cos(SphereYaw) * sin(SpherePitch)
	 * y = r * sin(SphereYaw) * sin(SpherePitch)
	 * z = r * cos(SpherePitch)
	 */

	// calculate random spawn point from sphere center
	const float SpherePitch =    FMath::FRandRange(-PI * 2.f, PI * 2.f);
	const float SphereYaw =      FMath::FRandRange(-PI * 2.f, PI * 2.f);
	const float CosSphereYaw =   cosf(SphereYaw);
	const float SinSphereYaw =   sinf(SphereYaw);
	const float CosSpherePitch = cosf(SpherePitch);
	const float SinSpherePitch = sinf(SpherePitch);

	const FVector SphericalOffset = FVector(
		SpawnRadius * CosSphereYaw * SinSpherePitch,
		SpawnRadius * SinSphereYaw * SinSpherePitch,
		SpawnRadius * CosSpherePitch
	);

	const FVector SpawnPoint = SpawnOrigin + SphericalOffset;

	// check if distance between nearest is bigger then threshold
	// recalculate new point if so

	// calculate rotation to face Origin
	const FVector AsteroidDirection = SpawnParam.AsteroidForwardVector.GetSafeNormal();
	

	// calculate scale

	return FTransform(FRotator(), SpawnPoint, FVector::OneVector);
}

void AAFPS_AsteroidSpawner::SpawnAsteroid()
{
	FTransform SpawnTransform = CalcNextSpawnTransform();
	FActorSpawnParameters SpawnParams;
	AAFPS_Asteroid* SpawnedAsteroid = GetWorld()->SpawnActor<AAFPS_Asteroid>(SpawnParam.AsteroidClass, SpawnTransform, SpawnParams);

	SpawnedAsteroids.Push(SpawnedAsteroid);
	NotifyAsteroidSpawned.Broadcast(SpawnedAsteroid);
}

FVector AAFPS_AsteroidSpawner::GetNewSpawnOrigin()
{
	FVector NewSpawnOrigin;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC->IsLocalController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				NewSpawnOrigin = Pawn->GetActorLocation();
				break;
			}
		}
	}

	return NewSpawnOrigin;
}

void AAFPS_AsteroidSpawner::OnAsteroidKilled(AAFPS_Asteroid* Asteroid, AActor* Killer, AController* KillerController)
{
	// remove if in SpawnedAsteroids container;
	SpawnedAsteroids.RemoveSingle(Asteroid);
	// if asteroid was destroyed and no OnAsteroidKilled is triggered, remove nullptrs
	SpawnedAsteroids.Remove(nullptr);

	if (--AsteroidToKillForNextWave <= 0)  // decrement asteroid to kill, check if we can start next wave
	{
		bAllowStartWave = true;
		StartNextWave();  // run next wave
	}
}


#if WITH_EDITOR
void AAFPS_AsteroidSpawner::DrawDebug(float DeltaSeconds)
{
	FString DbgMsg = "WaveCount: " + FString::FromInt(WaveCount)
		+ "\n SpawnRadius: " + FString::SanitizeFloat(SpawnRadius)
		+ "\n SpawnOrigin: " + SpawnOrigin.ToString()
		+ "\n NextWaveKillNeed: " + FString::FromInt(AsteroidToKillForNextWave)
		+ "\n AsteroidSpawnNum: " + FString::FromInt(AsteroidSpawnNum)
		+ "\n AsteroidScale: " + FString::SanitizeFloat(AsteroidScale)
		+ "\n SpawnedAsteroidsNum: " + FString::SanitizeFloat(SpawnedAsteroids.Num());

	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Cyan, DbgMsg, true);

	DrawDebugSphere(GetWorld(), SpawnOrigin, SpawnRadius, 36, FColor::Green);
}

void AAFPS_AsteroidSpawner::Tick(float DeltaSeconds)
{
	if (bDrawDebugAsteroidSpawn)
	{
		DrawDebug(DeltaSeconds);
	}
}
#endif // WITH_EDITOR
