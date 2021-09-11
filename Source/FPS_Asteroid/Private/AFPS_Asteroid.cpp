// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Asteroid.h"

#include <FPS_Asteroid/Public/Components/AFPS_HealthComponent.h>
#include "Engine/CollisionProfile.h"

// Sets default values
AAFPS_Asteroid::AAFPS_Asteroid()
{
	// create mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
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

	// enable physics, allow rotation only, disable gravity
	if (auto BI = MeshComp->GetBodyInstance())
	{
		BI->SetInstanceSimulatePhysics(true);
		BI->SetEnableGravity(false);
		BI->bLockXTranslation = true;
		BI->bLockYTranslation = true;
		BI->bLockZTranslation = true;
	}
}

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
