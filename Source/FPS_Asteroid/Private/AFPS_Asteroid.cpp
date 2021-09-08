// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Asteroid.h"

// Sets default values
AAFPS_Asteroid::AAFPS_Asteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

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

// Called when the game starts or when spawned
void AAFPS_Asteroid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFPS_Asteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

