// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AFPS_HealthComponent.h"
#include "GameFramework/Actor.h"

#include <FPS_Asteroid/Public/AFPS_GameMode.h>

// Sets default values for this component's properties
UAFPS_HealthComponent::UAFPS_HealthComponent()
{
	// defaults
	DefaultHealth = 100.f;
}

void UAFPS_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;

	// subscribe to actor TakeAnyDamageEvent to throw custom health changed event
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UAFPS_HealthComponent::HandleTakeAnyDamage);
	}
}

void UAFPS_HealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		if (AAFPS_GameMode* GM = GetWorld()->GetAuthGameMode<AAFPS_GameMode>())
		{
			GM->NotifyActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}

}
