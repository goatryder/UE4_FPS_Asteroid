// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_AsteroidSpawner.h"

#include "AFPS_Asteroid.h"
#include "AFPS_GameMode.h"

#include "Kismet/GameplayStatics.h"

AAFPS_AsteroidSpawner::AAFPS_AsteroidSpawner()
{
	#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	bDrawDebugAsteroidSpawn = true;
	#endif  // WITH_EDITOR

	// defaults
	SpawnParam.AsteroidClass = AAFPS_Asteroid::StaticClass();
	SpawnParam.InitialSpawnRadius = 15'00.f;  // 15m
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

	// todo


}

FTransform AAFPS_AsteroidSpawner::CalcNextSpawnTransform()
{
	// todo

	return FTransform();
}

void AAFPS_AsteroidSpawner::SpawnAsteroid()
{
	// todo

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
}

void AAFPS_AsteroidSpawner::Tick(float DeltaSeconds)
{
	if (bDrawDebugAsteroidSpawn)
	{
		DrawDebug(DeltaSeconds);
	}
}
#endif // WITH_EDITOR
