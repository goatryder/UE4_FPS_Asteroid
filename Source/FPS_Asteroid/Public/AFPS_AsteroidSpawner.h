// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_AsteroidSpawner.generated.h"

extern TAutoConsoleVariable<bool> CVarDrawDebugAsteroidSpawner;

class AAFPS_Asteroid;
class AAFPS_GameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsteroidSpawned, AAFPS_Asteroid*, Asteroid);

/**
 * Structure containing information about asteroid spawner spawn waves and initial wave spawn data
 */
USTRUCT(BlueprintType)
struct FAsteroidSpawnerParam
{
	GENERATED_BODY()

	/** Asteroid to spawn class */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAFPS_Asteroid> AsteroidClass;


	/** Distance from SpawnOrigin to spawn asteroid on first wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialSpawnRadius;

	/** Limit max SpawnRadius*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpawnRadius;

	/** Multiply SpawnRadius on next wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NextWaveRadiusMult;

	/** Asteroid number to spawn on first wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 InitialAsteroidSpawnNr;

	/** Stop asteroid spawning when total asteroid number on scene is >= then this value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpawnedAsteroidLimitMax;

	/** Multiply SpawnAstroidNr to spawn on next wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NextWaveAsteroidSpawnNrMult;


	/** Allow asteroid spawn if distance from other asteroid is >= this value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinSpawnDistanceBetweenAsteroids;

	/** Max attempts to adsjust sphere spawn position 
	 * to make sure it's at least at MinSpawnDistanceBetweenAsteroids from other spawned asteroids
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpawnPositionAdsjustAttemptsMax;

	/** Asteroid amount to destroy for triggering next spawn wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AsteroidKillNrToTriggerNextWave;


	/** Modify spawned asteroid scale, if asteroid count is ten, next scale will be 1.0 + AsteroidScaleStep * 10 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AsteroidScaleStep;
	
	/** Limit asteroid scale, will be min(step+scale, abs_limit) if step is positive and max(step+scale, abs_limit) is negative */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AsteroidScaleLimit;
};

UCLASS()
class FPS_ASTEROID_API AAFPS_AsteroidSpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	FAsteroidSpawnerParam SpawnParam;


	/** Current wave spawn stage */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	int32 WaveCount;

	/** Actual spawn radius, will change on each wave */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	float SpawnRadius;

	/** Spawn anchor, will change on each wave */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	FVector SpawnOrigin;

	/** Current asteroid to kill number left for triggering next wave spawn */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	int32 AsteroidToKillForNextWave;

	/** Current wave asteroid number to spawn */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	int32 AsteroidSpawnNum;

	/** Next spawned asteroid scale */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	float AsteroidScale;

	/** Store spawned asteroids to be able to calculate correct positions for next astroid spawn */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	TArray<AAFPS_Asteroid*> SpawnedAsteroids;

public:	
	// Sets default values for this actor's properties
	AAFPS_AsteroidSpawner();

	// Called from game mode in onStartPlay()
	void PrepareFirstWave(AAFPS_GameMode* GM);

	#if WITH_EDITOR
	void DrawDebug(float DeltaSeconds);
	virtual void Tick(float DeltaSeconds) override;
	#endif  // WITH_EDITOR

private:
	/** flag to block SpawnWave() when it's inappropriate */
	bool bAllowStartWave;

	/** check if existing Asteroids ammount is not exceeds SpawnParam.SpawnedAsteroidLimitMax */
	bool CanSpawnWave();

	/** proceed next wave spawn */
	void StartNextWave();

	/** Asteroids wave spawning */
	void StartWave();

	/** Calculate next random asteroid spawn point offset from sphere with anchor=SpawnOrigin, radius=SpawnRadius*/
	FORCEINLINE FVector CalcAsteroidSpawnPointOffset();

	/** Check if next spawn point is farther atleast then SpawnParam.MinSpawnDistanceBetweenAsteroids */
	FORCEINLINE bool IsAsteroidSpawnPointValid(const FVector& InSpawnPoint);

	// Calculate spawn transform depending on current wave params for single Asteroid instance
	virtual FTransform CalcAsteroidSpawnTransform();

	/*
	 * Spawn single Asteroid instance with calculated transform 
	 */
	void SpawnAsteroid();

	/** get location of zero index player controller's pawn of current world */
	FVector GetNewSpawnOrigin();

public:	
	UPROPERTY(BlueprintAssignable)
	FOnAsteroidSpawned NotifyAsteroidSpawned;

	/** Check if killed actor is asteroid, if so -> handle asteroid killed */
	UFUNCTION()
	void OnActorKilled(AActor* Victim, AActor* Killer, AController* KillerController);

	/** Get Wave Count */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE int32 GetWaveCount() const { return WaveCount; }

	/** Get asteroids to kill for next wave */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE int32 GetAsteroidToKillForNextWave() const { return AsteroidToKillForNextWave; }

	/** Get alive spawned asteroids */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE TArray<AAFPS_Asteroid*>& GetAliveSpawnedAsteroids() { return SpawnedAsteroids; }

};
