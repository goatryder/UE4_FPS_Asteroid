// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AFPS_GameMode.generated.h"

class AAFPS_Asteroid;
class AAFPS_AsteroidSpawner;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilledSignature, AActor*, Victim, AActor*, Killer, AController*, KillerController);

/**
 * 
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_GameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AFPS_GameMode", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAFPS_AsteroidSpawner> AsteroidSpawnerClass;

	UPROPERTY()
	AAFPS_AsteroidSpawner* AsteroidSpawner;

	/** Total Destroyed Asteroid Num */
	UPROPERTY()
	int32 KilledAsteroidNum;

	/** Total Spawned Asteroid Num from AsteroidSpawner */
	UPROPERTY()
	int32 SpawnedAsteroidNum;

public:
	AAFPS_GameMode();

	/** Transitions to calls BeginPlay on actors. */
	virtual void StartPlay() override;

	/** On Asteroid Killed Delegate, usually called from AAFPS_HealthComponent */
	UPROPERTY(BlueprintAssignable)
	FOnActorKilledSignature NotifyActorKilled;

	/** Get Total Destroyed Asteroids by player Num */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE int32 GetKilledAsteroidNum() const { return KilledAsteroidNum; }

	/** Get AsteroidSpawner */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE AAFPS_AsteroidSpawner* GetAsteroidSpawner() const { return AsteroidSpawner; }

	/** On Asteroid Destroy -> incr KilledAsteroidNum */
	UFUNCTION()
	void OnActorKilled(AActor* Victim, AActor* Killer, AController* KillerController);

	/** On Asteroid Spawned -> Incr SpawnedAsteroidNum */
	UFUNCTION()
	void OnAsteroidSpawned(AAFPS_Asteroid* Asteroid);

};
