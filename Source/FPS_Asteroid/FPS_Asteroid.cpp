// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Asteroid.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, FPS_Asteroid, "FPS_Asteroid" );

TAutoConsoleVariable<bool> CVarDrawDebugGlobal(
	TEXT("AFPS.DrawDebug.Global"),
	true,
	TEXT("Enable/Disable Global DrawDebug"),
	ECVF_Cheat
);