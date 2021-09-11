// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_AsteroidSpawner.h"

#include "AFPS_Asteroid.h"
#include "AFPS_GameMode.h"

#include "Kismet/GameplayStatics.h"

#include <FPS_Asteroid/FPS_Asteroid.h>

#include "DrawDebugHelpers.h"


TAutoConsoleVariable<bool> CVarDrawDebugAsteroidSpawner(
	TEXT("AFPS.DrawDebug.AsteroidSpawner"),
	true,
	TEXT("Enable/Disable AsteroidSpawner DrawDebug"),
	ECVF_Cheat
);

AAFPS_AsteroidSpawner::AAFPS_AsteroidSpawner()
{
	#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	#endif  // WITH_EDITOR

	// defaults
	SpawnParam.AsteroidClass = AAFPS_Asteroid::StaticClass();
	SpawnParam.InitialSpawnRadius = 25'00.f;  // 25m
	SpawnParam.MaxSpawnRadius = 1'000'00.f; // 1km
	SpawnParam.NextWaveRadiusMult = 1.05f;
	SpawnParam.InitialAsteroidSpawnNr = 15;
	SpawnParam.SpawnedAsteroidLimitMax = 1000;
	SpawnParam.NextWaveAsteroidSpawnNrMult = 1.1f;
	SpawnParam.MinSpawnDistanceBetweenAsteroids = 3'00.f;  // 3m 
	SpawnParam.SpawnPositionAdsjustAttemptsMax = 20;
	SpawnParam.AsteroidKillNrToTriggerNextWave = 10;
	SpawnParam.AsteroidScaleStep = -0.01;
	SpawnParam.AsteroidScaleLimit = 0.25;

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
		GM->NotifyActorKilled.AddDynamic(this, &AAFPS_AsteroidSpawner::OnActorKilled);

		// first wave spawn parameters
		WaveCount = 1;
		SpawnRadius = SpawnParam.InitialSpawnRadius;
		SpawnOrigin = GetNewSpawnOrigin();
		AsteroidToKillForNextWave = SpawnParam.AsteroidKillNrToTriggerNextWave;
		AsteroidSpawnNum = SpawnParam.InitialAsteroidSpawnNr;
		AsteroidScale = 1.0f;

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
		
		// run next wave
		StartWave();
	}
}

void AAFPS_AsteroidSpawner::StartWave()
{
	bAllowStartWave = false;
	
	//if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Red, "Start Next wave"); // debug

	// spawn asteroids
	for (int It = 0; It != AsteroidSpawnNum; ++It)
	{
		SpawnAsteroid();
	}
}

FORCEINLINE FVector AAFPS_AsteroidSpawner::CalcAsteroidSpawnPointOffset()
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
	const float SpherePitch = FMath::FRandRange(-PI * 2.f, PI * 2.f);
	const float SphereYaw = FMath::FRandRange(-PI * 2.f, PI * 2.f);
	const float CosSphereYaw = cosf(SphereYaw);
	const float SinSphereYaw = sinf(SphereYaw);
	const float CosSpherePitch = cosf(SpherePitch);
	const float SinSpherePitch = sinf(SpherePitch);

	const FVector SphericalOffset = FVector(
		SpawnRadius * CosSphereYaw * SinSpherePitch,
		SpawnRadius * SinSphereYaw * SinSpherePitch,
		SpawnRadius * CosSpherePitch
	);

	return SphericalOffset;
}

FORCEINLINE bool AAFPS_AsteroidSpawner::IsAsteroidSpawnPointValid(const FVector& InSpawnPoint)
{
	const float SquareDistTreshold = FMath::Square(SpawnParam.MinSpawnDistanceBetweenAsteroids);
	
	for (auto& Asteroid : SpawnedAsteroids)
	{
		if (Asteroid)
		{
			float AsteroidSquareDist = FVector::DistSquared(InSpawnPoint, Asteroid->GetActorLocation());
			if (AsteroidSquareDist < SquareDistTreshold)
			{
				// found one asteroid that is closer then SpawnParam.MinSpawnDistanceBetweenAsteroids
				return false;
			}
		}
	}
	return true;
}

FTransform AAFPS_AsteroidSpawner::CalcAsteroidSpawnTransform()
{
	// calculate asteroid spawn position
	FVector AsteroidOffsetFromSpawnAnchor;
	FVector AsteroidPosition;

	// we use here hack to limit max position adjusting attempts (we don't really want infinity loops)
	for (int32 Attempt = 0, MaxAttempt = SpawnParam.SpawnPositionAdsjustAttemptsMax; Attempt != MaxAttempt; ++Attempt)
	{
		AsteroidOffsetFromSpawnAnchor = CalcAsteroidSpawnPointOffset();
		AsteroidPosition = AsteroidOffsetFromSpawnAnchor + SpawnOrigin;
		if (IsAsteroidSpawnPointValid(AsteroidPosition))
		{
			// point is valid no need to calculate another one
			break;
		}

		#if WITH_EDITOR
		if (Attempt == MaxAttempt - 1) 
			UE_LOG(LogTemp, Warning, TEXT("[AsteroidSpawner] Can't adjust spawnPosition for Asteroid after %d attempts, spawn wherever"), SpawnParam.SpawnPositionAdsjustAttemptsMax);
		#endif // WITH_EDITOR
	}

	return FTransform(FRotator(), AsteroidPosition, FVector(AsteroidScale));
}

void AAFPS_AsteroidSpawner::SpawnAsteroid()
{
	FTransform SpawnTransform = CalcAsteroidSpawnTransform();
	AAFPS_Asteroid* SpawnedAsteroid = GetWorld()->SpawnActor<AAFPS_Asteroid>(SpawnParam.AsteroidClass, SpawnTransform);

	SpawnedAsteroids.Push(SpawnedAsteroid);
	NotifyAsteroidSpawned.Broadcast(SpawnedAsteroid);

	// Calculate next spawn asteroid scale
	AsteroidScale = SpawnParam.AsteroidScaleStep > 0.f ?
		FMath::Min(AsteroidScale + SpawnParam.AsteroidScaleStep, FMath::Abs(SpawnParam.AsteroidScaleLimit)) : 
		FMath::Max(AsteroidScale + SpawnParam.AsteroidScaleStep, FMath::Abs(SpawnParam.AsteroidScaleLimit));
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

void AAFPS_AsteroidSpawner::OnActorKilled(AActor* Victim, AActor* Killer, AController* KillerController)
{
	if (AAFPS_Asteroid* Asteroid = Cast<AAFPS_Asteroid>(Victim))
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
}


#if WITH_EDITOR
void AAFPS_AsteroidSpawner::DrawDebug(float DeltaSeconds)
{
	// spawn sphere
	DrawDebugSphere(GetWorld(), SpawnOrigin, SpawnRadius, 36, FColor::Green);

	// params
	FString DbgMsg = "WaveCount: " + FString::FromInt(WaveCount)
		+ "\n SpawnRadius: " + FString::SanitizeFloat(SpawnRadius)
		+ "\n SpawnOrigin: " + SpawnOrigin.ToString()
		+ "\n NextWaveKillNeed: " + FString::FromInt(AsteroidToKillForNextWave)
		+ "\n AsteroidSpawnNum: " + FString::FromInt(AsteroidSpawnNum)
		+ "\n AsteroidNextScale: " + FString::SanitizeFloat(AsteroidScale)
		+ "\n SpawnedAsteroidsNum: " + FString::FromInt(SpawnedAsteroids.Num());

	if (auto PC = GetWorld()->GetFirstPlayerController())
	{
		FVector ViewPoint;
		FRotator ViewRotation;
		PC->GetActorEyesViewPoint(ViewPoint, ViewRotation);
		
		static FVector MsgCameraOffset(100.f, 55.f, 45.f);
		const FRotationMatrix ViewRotMatrix(ViewRotation);
		
		FVector DbgMsgDrawLoc = ViewPoint + ViewRotMatrix.TransformPosition(MsgCameraOffset);

		DrawDebugString(GetWorld(), DbgMsgDrawLoc, DbgMsg, 0, FColor::Orange, 0.f, true);
	}
}

void AAFPS_AsteroidSpawner::Tick(float DeltaSeconds)
{
	if (CVarDrawDebugAsteroidSpawner.GetValueOnGameThread() && 
		CVarDrawDebugGlobal.GetValueOnGameThread())
	{
		DrawDebug(DeltaSeconds);
	}
}
#endif // WITH_EDITOR
