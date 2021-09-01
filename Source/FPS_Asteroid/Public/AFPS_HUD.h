// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "AFPS_HUD.generated.h"

/**
 * 
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_HUD : public AHUD
{
	GENERATED_BODY()
	
	/** crosshair icon */
	UPROPERTY()
	FCanvasIcon CrosshairIcon;

public:
	AAFPS_HUD();

	/** Main HUD update loop. */
	virtual void DrawHUD() override;

};
