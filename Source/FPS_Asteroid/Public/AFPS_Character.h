// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AFPS_Character.generated.h"

class UCameraComponent;

UCLASS()
class FPS_ASTEROID_API AAFPS_Character : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	/** First person skeletal mesh */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1PComp;

public:
	// Sets default values for this character's properties
	AAFPS_Character();

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

	FORCEINLINE USkeletalMeshComponent* GetMesh1P() { return Mesh1PComp; }

private:
	FORCEINLINE void DrawDebug();
};
