// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Weapon.h"
#include "Engine/CollisionProfile.h"

// Sets default values
AAFPS_Weapon::AAFPS_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RootComponent = MeshComp;

	// find default mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun.SK_FPGun"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetSkeletalMesh(MeshFinder.Object);
	}

	// weapon defaults
	MuzzleSocketName = "Muzzle";
	AttachSocketName = "GripPoint";

	Range = 1'000'00.f;  // 1000 m
	Damage = 10.f;
	FireRate = 0.1f;
	EnergyLevel = 100.f;
	EnergyRecoveryRate = 50.f;
	EnergyDrainPerShot = 2.5f;

	DamageType = UDamageType::StaticClass();

	TraceEffectSource = "Source";
	TraceEffectTarget = "Target";

}

// Called when the game starts or when spawned
void AAFPS_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFPS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		DrawDebug();
	#endif

}

void AAFPS_Weapon::StartFire()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Magenta, "StartFire()");
}

void AAFPS_Weapon::StopFire()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Magenta, "StopFire()");
}

void AAFPS_Weapon::OnAttach(AActor* ActorOwner)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Magenta, "OnAttach()");

	AttachedTo = ActorOwner;
	SetOwner(ActorOwner);
}

bool AAFPS_Weapon::CanFire()
{
	return false;
}

void AAFPS_Weapon::StartFire_Internal()
{

}

void AAFPS_Weapon::FireLoop()
{

}

void AAFPS_Weapon::PlayStartFireEffects()
{

}

void AAFPS_Weapon::PlayFireEffects()
{

}

void AAFPS_Weapon::PlayEnergyDrainedEffects()
{

}

void AAFPS_Weapon::PlayEnergyRestoredEffects()
{

}

FORCEINLINE void AAFPS_Weapon::DrawDebug()
{

}
