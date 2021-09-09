// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "AFPS_HUD.generated.h"

class AAFPS_Character;
class AAFPS_GameMode;

/**
 * 
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_HUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;

	/** crosshair icon */
	UPROPERTY()
	FCanvasIcon CrosshairIcon;

	/** Player Character HUD Owner */
	UPROPERTY()
	AAFPS_Character* Character;

	/** Custom GameMode cache */
	UPROPERTY()
	AAFPS_GameMode* GM;

public:
	AAFPS_HUD();
	
	/** Main HUD update loop. */
	virtual void DrawHUD() override;


	/** Get from GameMode AsteroidSpawner current wave count */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetAsteroidSpawnWaveCount();

	/** Get from GameMode AsteroidSpawner asteroid num to destroy to trigger next wave */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetAsteroidToKillForNextWave();

	/** Get from GameMode AsteroidSpawner alive spawned asteroid num */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetAliveSpawnedAsteroidNum();

	/** Get from GameMode Total Killed asteroid num */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetKilledAsteroidNum();


	/** Get character weapon energy level normalized */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetCharacterWeaponEnergyLevelAlpha();

protected:
	virtual void BeginPlay() override;

};
