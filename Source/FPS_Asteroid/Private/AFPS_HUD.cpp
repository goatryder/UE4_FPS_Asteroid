// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_HUD.h"

#include <FPS_Asteroid/Public/AFPS_Character.h>
#include <FPS_Asteroid/Public/AFPS_Weapon.h>
#include <FPS_Asteroid/Public/AFPS_GameMode.h>
#include <FPS_Asteroid/Public/AFPS_AsteroidSpawner.h>

AAFPS_HUD::AAFPS_HUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTextureObjFinder(TEXT("/Game/FPSLaserGun/Textures/FirstPersonCrosshair"));
	if (CrosshairTextureObjFinder.Succeeded())
	{
		CrosshairTexture = CrosshairTextureObjFinder.Object;
	}
}

void AAFPS_HUD::BeginPlay()
{
	Super::BeginPlay();

	if (CrosshairTexture)
	{
		CrosshairIcon = UCanvas::MakeIcon(CrosshairTexture, 0.f, 0.f, CrosshairTexture->GetSizeX(), CrosshairTexture->GetSizeY());
	}

	// get gamemode
	GM = Cast<AAFPS_GameMode>(GetWorld()->GetAuthGameMode<AAFPS_GameMode>());
}

void AAFPS_HUD::DrawHUD()
{
	if (Canvas)
	{
		float CanvasCenterX, CanvasCenterY;
		Canvas->GetCenter(CanvasCenterX, CanvasCenterY);
		
		Canvas->DrawIcon(CrosshairIcon, CanvasCenterX - CrosshairIcon.UL * 0.5f, CanvasCenterY - CrosshairIcon.VL * 0.5f, 1.f);
	}

	Super::DrawHUD();
}

int32 AAFPS_HUD::GetAsteroidSpawnWaveCount()
{
	if (GM)
	{
		if (auto AsteroidSpawner = GM->GetAsteroidSpawner())
		{
			return AsteroidSpawner->GetWaveCount();
		}
	}

	return 0;
}

int32 AAFPS_HUD::GetAsteroidToKillForNextWave()
{
	if (GM)
	{
		if (auto AsteroidSpawner = GM->GetAsteroidSpawner())
		{
			return AsteroidSpawner->GetAsteroidToKillForNextWave();
		}
	}

	return 0;
}

int32 AAFPS_HUD::GetAliveSpawnedAsteroidNum()
{
	if (GM)
	{
		if (auto AsteroidSpawner = GM->GetAsteroidSpawner())
		{
			auto& SpawnedAsteroids = AsteroidSpawner->GetAliveSpawnedAsteroids();

			return SpawnedAsteroids.Num();
		}
	}

	return 0;
}

int32 AAFPS_HUD::GetKilledAsteroidNum()
{
	if (GM)
	{
		return GM->GetKilledAsteroidNum();
	}

	return 0;
}

float AAFPS_HUD::GetCharacterWeaponEnergyLevelAlpha()
{
	if (Character)
	{
		if (auto Weapon = Character->GetWeaponInHands())
		{
			return Weapon->GetEnergyLevelAlpha();
		}
	}
	else
	{
		// get character
		Character = Cast<AAFPS_Character>(GetOwningPawn());
		if (Character)
		{
			GetCharacterWeaponEnergyLevelAlpha();
		}
	}

	return 0.f;
}