// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../FPS_Asteroid.h"
#include "AFPS_Character.generated.h"

class UCameraComponent;
class UAnimMontage;
class AAFPS_Weapon;

USTRUCT(BlueprintType)
struct FMeshRotationLag
{
	GENERATED_BODY()

	// max absolute pitch degree of mesh rotation
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.0f, ClampMax = 20.0f))
	float MaxPitchLag = 9.f;

	// max absolute yaw degree of mesh rotation
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.0f, ClampMax = 20.0f))
	float MaxYawLag = 15.f;

	// Lag speed, degree per second
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.01f, ClampMax = 4.0f))
	float LagSpeed = 0.6f;

	// store mesh to apply lag rotation on
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	// default mesh relative pitch
	float InitialPitch;
	// default mesh relative yaw
	float InitialYaw;

	// current yaw rotation target
	float YawTo;
	// current pitch rotation target
	float PitchTo;
	
	float LastMouseInput_Pitch;
	float LastMouseInput_Yaw;

	// set Mesh to apply rotation, save initial pitch and yaw values
	void Initialize(USkeletalMeshComponent* MeshToRotate);
	
	// handle mouse Pitch input to apply mesh rotation lag
	void OnUpdateInputPitch(float InputPitch);

	// handle mouse Yaw input to apply mesh rotation lag
	void OnUpdateInputYaw(float InputYaw);
};

UCLASS()
class FPS_ASTEROID_API AAFPS_Character : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	/** First person skeletal mesh */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1PComp;


	/** character sk mesh hand socket where weapon will be attached */
	UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly)
	FName WeaponSocketName;

	/** character default weapon type */
	UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly)
	TSubclassOf<AAFPS_Weapon> DefaultWeaponClass;

	/** weapon shooting animation */
	UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly)
	UAnimMontage* FireAnimMontage;

	/** enable/disable mesh rotation lag feature */
	UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly)
	bool bEnableMeshRotationLag;

	/** mesh rotation lag parameters*/
	UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly, meta = (EditCondition = "bEnableMeshRotationLag"))
	FMeshRotationLag MeshLagParams;

	// look point trace channel
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FPSCharacter", meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<ECollisionChannel> LookLineTraceChannel;

	// cache last view point trace result
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		mutable FHitResult LookTrace;

#if WITH_EDITORONLY_DATA
		/** enable/disable character draw debug, EDITOR ONLY */
		UPROPERTY(Category = "FPSCharacter", EditDefaultsOnly)
		bool bDrawDebugCharacter;
#endif

	/** character current weapon */
	UPROPERTY()
	AAFPS_Weapon* WeaponInHands;

	/** Caching inputs values for use in anim instance */
	float LastForwardInput, LastRightInput, LastUpInput;

	// default look point trace params
	FCollisionQueryParams LookTraceQueryParams;

public:
	// Sets default values for this character's properties
	AAFPS_Character();

	/**
	* spawn weapon from DefaultWeaponClass
	*
	* @param bDestroyOldWeapon true: has weapon -> destroy then spawn new. false: has weapon -> return;
	*/
	UFUNCTION(BlueprintCallable, Category = "FPSCharacter")
	void SpawnWeaponAttached(bool bDestroyOldWeapon = false);

	/** Play Weapon shooting anim monatge */
	UFUNCTION(BlueprintCallable, Category = "FPSCharacter")
	void PlayFireAnimMontage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	* fly forward/back
	*
	* @param Val Movment input to apply
	*/
	void FlyForward(float Val);

	/**
	* fly right/left
	*
	* @param Val Movment input to apply
	*/
	void FlyRight(float Val);

	/**
	* fly up/down
	*
	* @param Val Movment input to apply
	*/
	void FlyUp(float Val);

	/**
	* mouse input pitch
	*
	* @param PitchInput value to apply
	*/
	void LookUpInput(float PitchInput);

	/**
	* mouse input yaw
	*
	* @param YawInput value to apply
	*/
	void TurnInput(float YawInput);

	/** player pressed fire action */
	void OnStartFire();

	/** player released fire action */
	void OnStopFire();

	/** Get first person mesh */
	UFUNCTION(BlueprintCallable, Category = Character)
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComp; }

	/** Get character current weapon */
	UFUNCTION(BlueprintCallable, Category = "FPSCharacter")
	FORCEINLINE AAFPS_Weapon* GetWeaponInHands() const { return WeaponInHands; }

	/** Get character sk mesh hand socket where weapon will be attached */
	UFUNCTION(BlueprintCallable, Category = "FPSCharacter")
	FORCEINLINE FName GetWeaponSocketName() const { return WeaponSocketName; }

	/** Get Last Forward Input value */
	FORCEINLINE float GetLastForwardInput() const { return LastForwardInput; }

	/** Get Last Right Input value */
	FORCEINLINE float GetLastRightInput() const { return LastRightInput; }

	/** Get Last Up Input value */
	FORCEINLINE float GetLastUpInput() const { return LastUpInput; }

	/** get look point trace result */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Weapon")
	FORCEINLINE FHitResult& GetLookTraceResult() const { return LookTrace; }

	/** Get character veiw point, if trace fail -> get far max view point */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = Character)
	FORCEINLINE FVector GetCharacterViewPoint() const
	{
		return LookTrace.IsValidBlockingHit() ? LookTrace.Location : GetFarLookPoint();
	}

private:
	FORCEINLINE void DrawDebug();

	/** Get max allowed character look focus point
	 * Returns eyes_location + eyes_direction * TRACE_DIST_MAX
	 */
	FORCEINLINE FVector GetFarLookPoint() const
	{
		FVector EyeLoc;
		FRotator EyeRot;
		GetActorEyesViewPoint(EyeLoc, EyeRot);

		FVector LookPointMax = EyeLoc + EyeRot.Vector() * TRACE_DIST_MAX;
		return LookPointMax;
	}

	/** Get max allowed character look focus line
	*/
	FORCEINLINE void GetLookTraceLine(FVector& OutLookPointStart, FVector& OutLookPointEnd) const
	{
		FRotator EyeRot;
		GetActorEyesViewPoint(OutLookPointStart, EyeRot);
		OutLookPointEnd = OutLookPointStart + EyeRot.Vector() * TRACE_DIST_MAX;
	}

	/** OnTick look point trace */
	FORCEINLINE void LookPointTrace()
	{
		FVector TraceStart;
		FVector TraceEnd;
		GetLookTraceLine(TraceStart, TraceEnd);
		GetWorld()->LineTraceSingleByChannel(LookTrace, TraceStart, TraceEnd, LookLineTraceChannel, LookTraceQueryParams);
	}

};
