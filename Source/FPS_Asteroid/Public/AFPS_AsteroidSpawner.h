// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_AsteroidSpawner.generated.h"

class AAFPS_Asteroid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsteroidSpawned, AAFPS_Asteroid*, Asteroid);

UCLASS()
class FPS_ASTEROID_API AAFPS_AsteroidSpawner : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAFPS_AsteroidSpawner();

	UPROPERTY(EditDefaultsOnly, Category = "AsteroidSpawner")
	TSubclassOf<AAFPS_Asteroid> AsteroidClass;

	UPROPERTY(EditDefaultsOnly, Category = "AsteroidSpawner")
	float InitialSpawnRadius;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual FTransform CalcNextSpawnTransform();

public:	
	UPROPERTY(BlueprintAssignable)
	FOnAsteroidSpawned NotifyAsteroidSpawned;

	/** Spawn Asteroid with calculated transform */
	UFUNCTION(BlueprintCallable)
	void SpawnAsteroid();

};
