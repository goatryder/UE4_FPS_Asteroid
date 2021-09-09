// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AFPS_HealthComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UAFPS_HealthComponent*, HealthComp, 
float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_ASTEROID_API UAFPS_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	float DefaultHealth;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

public:	
	// Sets default values for this component's properties
	UAFPS_HealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
	
	/** TakeDamageResponse */
	UPROPERTY(BlueprintAssignable, Category = "HealthComponent")
	FOnHealthChangedSignature OnHealthChanged;

	/** Get actor health */
	FORCEINLINE float GetHealth() const { return Health; }

	/** Get actor default max health */
	FORCEINLINE float GetDefaultHealth() const { return DefaultHealth; }

	/** return current health normalized value from 0.0 to 1.0 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "HealthComponent")
	FORCEINLINE float GetHealthAlpha() const { return FMath::Clamp(Health / DefaultHealth, 0.f, 1.f); }

	/** Check if actor should be dead */
	FORCEINLINE bool IsDead() const { return bIsDead; }

};
