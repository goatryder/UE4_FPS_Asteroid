// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_AsteroidSpawner.generated.h"

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


	/** Multiply Spawner Asteroid scale on next wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NextWaveAsteroidScaleMult;

	/** Stop scaling asteroids when scale is exceeds this limit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AsteroidScaleLimit;


	/** Allow asteroid spawn if distance from other asteroid is >= this value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinSpawnDistanceBetweenAsteroids;


	/** Asteroid amount to destroy for triggering next spawn wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AsteroidKillNrToTriggerNextWave;

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

	/** Current wave asteroid scale, will change on each wave */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	float AsteroidScale;

	/** Store spawned asteroids to be able to calculate correct positions for next astroid spawn */
	UPROPERTY(BlueprintReadOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	TArray<AAFPS_Asteroid*> SpawnedAsteroids;

	#if WITH_EDITORONLY_DATA
	/** enable/disable AsteroidSpawner draw debug, EDITOR ONLY */
	UPROPERTY(EditDefaultsOnly, Category = "AsteroidSpawner", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugAsteroidSpawn;
	#endif  // WITH_EDITORONLY_DATA

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

	// Calculate spawn transform depending on current wave params for single Asteroid instance
	virtual FTransform CalcNextSpawnTransform();

	/** Spawn single Asteroid instance with calculated transform */
	void SpawnAsteroid();

	/** get location of zero index player controller's pawn of current world */
	FVector GetNewSpawnOrigin();


	/** AFPS_GameMode FOnAsteroidKilled delegate callback */
	UFUNCTION()
	void OnAsteroidKilled(AAFPS_Asteroid* Asteroid, AActor* Killer, AController* KillerController);

public:	
	UPROPERTY(BlueprintAssignable)
	FOnAsteroidSpawned NotifyAsteroidSpawned;

};
