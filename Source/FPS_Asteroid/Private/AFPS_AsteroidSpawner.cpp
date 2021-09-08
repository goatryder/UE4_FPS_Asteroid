// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_AsteroidSpawner.h"

#include "AFPS_Asteroid.h"

AAFPS_AsteroidSpawner::AAFPS_AsteroidSpawner()
{
	// defaults
	InitialSpawnRadius = 20'00.f; // 20m

	AsteroidClass = AAFPS_Asteroid::StaticClass();
}

void AAFPS_AsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

FTransform AAFPS_AsteroidSpawner::CalcNextSpawnTransform()
{
	return FTransform();
}

void AAFPS_AsteroidSpawner::SpawnAsteroid()
{
	FActorSpawnParameters SpawnParams;
	FTransform SpawnTransform = CalcNextSpawnTransform();

	GetWorld()->SpawnActor<AAFPS_Asteroid>(AsteroidClass, SpawnTransform, SpawnParams);
}
