// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Asteroid.h"

#include <FPS_Asteroid/Public/Components/AFPS_HealthComponent.h>
#include "C:/Program Files/Epic Games/UE_4.26/Engine/Plugins/Runtime/Analytics/Adjust/Source/AndroidAdjust/Private/AndroidAdjustProvider.h"

// Sets default values
AAFPS_Asteroid::AAFPS_Asteroid()
{
	#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	#endif

	// create mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

	// health component
	HealthComp = CreateDefaultSubobject<UAFPS_HealthComponent>(TEXT("Health"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AAFPS_Asteroid::OnHealthChanged);

	// mesh find
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/FPSAsteroid/SM_Rock.SM_Rock"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshFinder.Object);
	}

	// enable physics, allow rotation, disable gravity
	if (auto BI = MeshComp->GetBodyInstance())
	{
		BI->SetInstanceSimulatePhysics(true);
		BI->SetEnableGravity(false);
		BI->bLockXTranslation = true;
		BI->bLockYTranslation = true;
		BI->bLockZTranslation = true;
	}
}

#if WITH_EDITOR
void AAFPS_Asteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// debug
	if (bDrawDebugAsteroidSpawn)
	{
		DrawDebug(DeltaTime);
	}
}

void AAFPS_Asteroid::DrawDebug(float DeltaSeconds)
{

}
#endif // WITH_EDITOR


void AAFPS_Asteroid::OnHealthChanged(UAFPS_HealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (InHealthComp)
	{
		if (InHealthComp->IsDead())
		{
			OnAsteroidDeath();
		}
	}
}

void AAFPS_Asteroid::OnAsteroidDeath_Implementation()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow, "Asteroid Is Killed");

	Destroy();
}
