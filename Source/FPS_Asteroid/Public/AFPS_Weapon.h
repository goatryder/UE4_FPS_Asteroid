// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_Weapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class USoundBase;

//=============================================================================
/**
 * This is not exactly base class for weapons
 * this is more like only one weapon in project
 * It's should looks like some kind of lazer beam gun
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_Weapon : public AActor
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(Category = "Weapon", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MeshComp;

	// place where we can visualise some debug info
	FORCEINLINE void DrawDebug();

	// timer to handle fire logic
	FTimerHandle TimerHandle_FireLoop;

	// default weapon line trace params
	FCollisionQueryParams ShotTraceQueryParams;

	/** Actor who has this weapon attached to itself */
	UPROPERTY()
	AActor* AttachedTo;
	
public:	
	// Sets default values for this actor's properties
	AAFPS_Weapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Starts weapon firing */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	/** Stops weapon firing */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	/** should be called when attached to character */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void OnAttach(AActor* ActorOwner);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** spawn fx effects place */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName;

	/** attach weapon to character hand socket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName AttachSocketName;

	//UPROPERTY(EditDefaultsOnly)
	ECollisionChannel ShotLineTraceChanel;

	/** Max weapon range to trace hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Range;

	/** Base weapon damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Damage;

	/** Weapon start fire first delay */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float StartFireDelay;

	/**  Weapon Fire rate in sec. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float FireRate;

	/**  Weapon Energy level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyLevel;

	/** Weapon Energy recovery per second when not shooting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyRecoveryRate;

	/** Weapon Energy loosing while shooting, descrese energy level each fire loop */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyDrainPerShot;

	/** Weapon damage class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<UDamageType>  DamageType;

	// todo:  add camera shake

	/** Sound to play when weapons starts to fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: Sound")
	USoundBase* StartFireSound;

	/** Sound to play when weapon fire loop continue */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: Sound")
	USoundBase* FireSound;

	/** Sound to play when weapon fire loop continue */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: Sound")
	USoundBase* EnergyDrainedSound;

	/** Sound to play when weapon energy was fully drained and then restored */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: Sound")
	USoundBase* EnergyRestoredSound;

	/** Particle to spawn when weapon starts to fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX")
	UParticleSystem* StartFireEffect;

	/** Particle with source and target to add visual fire trace effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX")
	UParticleSystem* TraceEffect;

	/** Name of particle TraceEffect source */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX", meta = (EditCondition = "TraceEffect"))
	FName TraceEffectSource;

	/** Name of particle TraceEffect targe*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX", meta = (EditCondition = "TraceEffect"))
	FName TraceEffectTarget;

	/** Particle to spawn on impact point */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX")
	UParticleSystem* ImpactEffect;

	/** Spawn attached particle to muzzle when weapon energy is drained */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon: FX")
	UParticleSystem* EnergyDrainedEffect;


	/** True if fire key pressed */
	bool bWantsToFire;

	/** True when Energy level was fully drained, set back to false on energy restored */
	bool bEnergyDrained;

	/** check if weapon allowed to fire */
	bool CanFire();

	/** Start Fire logic */
	void StartFire_Internal();

	/** Fire loop logic */
	void FireLoop();

	/** Effects when weapons starts to fire */
	void PlayStartFireEffects();

	/** Play weapon fire effects while shooting is looping */
	void PlayFireEffects();

	/** Play weapon effects when energy is drained */
	void PlayEnergyDrainedEffects();

	/** Play weapon effects when energy level is fully restored after it was drained */
	void PlayEnergyRestoredEffects();

public:
	/** get actor who has this weapon attached to itself */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE AActor* GetAttachedToActor() { return AttachedTo; }

	/** get weapon skeletal mesh */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE USkeletalMeshComponent* GetMesh() { return MeshComp; }

	/** get weapon attach socket name */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE FName GetAttachSocketName() { return AttachSocketName; }

	/** get weapon muzzle socket name */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FORCEINLINE FName GetMuzzleSocketName() { return MuzzleSocketName; }

};
