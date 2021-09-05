// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_HUD.h"

AAFPS_HUD::AAFPS_HUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTextureObjFinder(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	UTexture2D* CrosshairTexture = CrosshairTextureObjFinder.Object;

	CrosshairIcon = UCanvas::MakeIcon(CrosshairTexture, 0.f, 0.f, CrosshairTexture->GetSizeX(), CrosshairTexture->GetSizeY());
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