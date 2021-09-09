// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_GameMode.h"

#include "AFPS_AsteroidSpawner.h"

AAFPS_GameMode::AAFPS_GameMode()
{
	// defaults
	AsteroidSpawnerClass = AAFPS_AsteroidSpawner::StaticClass();
}

void AAFPS_GameMode::StartPlay()
{
	Super::StartPlay();

	// create asteroid spawner instance
	AsteroidSpawner = GetWorld()->SpawnActor<AAFPS_AsteroidSpawner>(AsteroidSpawnerClass);
	if (AsteroidSpawner)
	{
		AsteroidSpawner->PrepareFirstWave(this);

		// debug
		if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Green, "GM Prep first wave");
	}
}

void AAFPS_GameMode::OnActorKilled(AActor* Victim, AActor* Killer, AController* KillerController)
{
	++KilledAsteroidNum;
}

void AAFPS_GameMode::OnAsteroidSpawned(AAFPS_Asteroid* Asteroid)
{
	++SpawnedAsteroidNum;
}
