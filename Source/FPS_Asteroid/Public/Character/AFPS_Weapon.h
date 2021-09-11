// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_Weapon.generated.h"

extern TAutoConsoleVariable<bool> CVarDrawDebugWeapon;

class USkeletalMeshComponent;
class AAFPSCharacter;

//=============================================================================
/**
 * This is not exactly base class for weapons
 * this is more like only one weapon in project
 * It's should looks like some kind of lazer beam gun
 * effects can be added in blueprint through blueprintImplementableEvents
 */
UCLASS()
class FPS_ASTEROID_API AAFPS_Weapon : public AActor
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(Category = "Weapon", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MeshComp;

	// place where we can visualise some debug info
	FORCEINLINE void DrawDebug(float DeltaSeconds);

	// default weapon hit line trace params
	FCollisionQueryParams HitTraceQueryParams;

	/** AFPSChracter who has this weapon attached to itself */
	UPROPERTY()
	AAFPS_Character* CharacterOwner;

	// cache last hit result to allow PlayEffects function receive hit info
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	mutable FHitResult LastHit;

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
	void OnAttach(AAFPS_Character* InCharacterOwner);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** spawn fx effects place */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName;

	/** attach weapon to character hand socket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TEnumAsByte<ECollisionChannel> ShotLineTraceChannel;

	/** Max weapon range to trace hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Range;

	/** Base weapon damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Damage;

	/** Weapon start fire first delay, using to block fire button click spam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float StartFireDelay;

	/**  Weapon Fire rate, seconds to make next shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float FireRate;

	/**  Weapon Energy level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyLevel;

	/** Weapon Energy points recovery per second when not shooting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyRecoveryRate;

	/** Weapon Energy loosing while shooting, descrese energy level each fire loop */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float EnergyDrainPerShot;

	/** Weapon damage class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<UDamageType>  DamageType;

private:
	// timer to handle fire logic
	FTimerHandle TimerHandle_FireLoop;

	/** True if fire key pressed */
	bool bWantsToFire;

	/** True if energy was fully drained and need to play energy restoration effect */
	bool bEnergyWasDrained;

	/** True if actual fire loop is running */
	bool bIsFiring;

	/** Energy level next target, used to smooth weapon energy level changing */
	float EnergyLevelTarget;

	/** Actual energy level of weapon, calculated each frame*/
	float CurrentEnergyLevel;

	/** Last time when firing was instigated, used to disallow fire button spamming */
	float LastTimeWhenFiringStarts;

	/** check if time between last shoot try is > StartFireDelay */
	bool CanStartShooting();

	/** check if weapon energy allows enouph to single shot */
	bool HasEnergyForSingleShot();


	/** Start Fire logic */
	void StartFire_Internal();

	/** Function to execute delayed single fire shot logic or to stop it 
	 * @param bShouldReset if true - reset timer, false - cancel timer
	 */
	void ResetShotTimer(bool bShouldReset);
	/** Single Shot linetrace handling */
	void ShotLineTrace();
	/** Fire loop logic */
	void FireLoop();

	/** Stop fire logic. Executed when bWantsToFire is become false again */
	void StopFire_Internal();

	/** Frame-per-frame Tick energy calculation, decreased when firing, increased when not firining */
	FORCEINLINE void OnTickCalculateEnergyLevel(float DeltaSeconds);

protected:
	//=============================================================================
	/* Blueprint implementable weapon fx */

	/** Play effects when weapon starts to fire BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayStartFireEffects();

	/**  Play effects when weapon starts to fire (calls blueprint implemented) */
	virtual void PlayStartFireEffects();

	/** Play weapon effects when not shooting any more BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayEndFireEffects();

	/** Play weapon effects when not shooting any more (calls blueprint implemented) */
	virtual void PlayEndFireEffects();

	/** Play weapon fire effects while shooting is looping BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayFireEffects();

	/** Play weapon fire effects while shooting is looping (calls blueprint implemented) */
	virtual void PlayFireEffects();

	/** Play weapon impact effects when hit detected BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayHitEffects();

	/** Play weapon impact effects when hit detected (calls blueprint implemented) */
	virtual void PlayHitEffects();

	/** Play weapon effects when energy is drained BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayEnergyDrainedEffects();

	/** Play weapon effects when energy is drained (calls blueprint implemented) */
	virtual void PlayEnergyDrainedEffects();

	/** Play weapon effects when energy level is fully restored after it was drained BLUEPRINT IMPLEMENTABLE */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnPlayEnergyRestoredEffects();

	/** Play weapon effects when energy level is fully restored after it was drained (calls blueprint implemented) */
	virtual void PlayEnergyRestoredEffects();


public:
	/** get character who has this weapon attached to itself */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE AAFPS_Character* GetCharacterAttachedTo() const { return CharacterOwner; }

	/** get weapon skeletal mesh */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return MeshComp; }

	/** get weapon attach socket name */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE FName GetAttachSocketName() const { return AttachSocketName; }

	/** get weapon muzzle socket name */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE FName GetMuzzleSocketName() const { return MuzzleSocketName; }

	/** get weapon current energy level */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE float GetCurrentEnergyLevel() const { return CurrentEnergyLevel; }

	/** get normalized current energy level from 0.0 to 1.0 where 1.0 when its equal to default EnergyLevel */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE float GetEnergyLevelAlpha() const { return FMath::Clamp(CurrentEnergyLevel / EnergyLevel, 0.f, 1.f); }

	/** get weapon last hit trace result */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE FHitResult& GetLastShotHitTraceResult() const { return LastHit; }
};
