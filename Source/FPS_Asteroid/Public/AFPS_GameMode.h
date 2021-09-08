// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AFPS_GameMode.generated.h"

class AAFPS_Asteroid;
class AAFPS_AsteroidSpawner;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAsteroidKilled, AAFPS_Asteroid*, Asteroid, AActor*, Killer, AController*, KillerController);

/**
 * 
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_GameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AFPS_GameMode", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAFPS_AsteroidSpawner> AsteroidSpawnerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AAFPS_AsteroidSpawner* AsteroidSpawner;

public:
	AAFPS_GameMode();

	/** Transitions to calls BeginPlay on actors. */
	virtual void StartPlay() override;
	
	/** When asteroid is killed it will broadcast this delegate */
	UPROPERTY(BlueprintAssignable)
	FOnAsteroidKilled NotifyAsteroidKilled;
};
