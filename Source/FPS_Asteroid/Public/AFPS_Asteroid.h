// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_Asteroid.generated.h"

class UAFPS_HealthComponent;

UCLASS()
class FPS_ASTEROID_API AAFPS_Asteroid : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Components, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
	
	/** Simple health component for handling damage */
	UPROPERTY(Category = Components, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAFPS_HealthComponent* HealthComp;

	#if WITH_EDITORONLY_DATA
	/** enable/disable Asteroid draw debug, EDITOR ONLY */
	UPROPERTY(EditDefaultsOnly, Category = "Asteroid", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugAsteroid;
	#endif  // WITH_EDITORONLY_DATA

public:	
	// Sets default values for this actor's properties
	AAFPS_Asteroid();

	/** On Health component changing health callback */
	UFUNCTION()
	void OnHealthChanged(UAFPS_HealthComponent* InHealthComp, float Health, float HealthDelta, 
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** This is blueprint event to handle asteroid death fx and destory after */
	UFUNCTION(BlueprintNativeEvent)
	void OnAsteroidDeath();
};
