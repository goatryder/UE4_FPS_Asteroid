// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AFPS_CharacterAnim.generated.h"

class AAFPS_Character;

/**
 * 
 */
UCLASS()
class FPS_ASTEROID_API UAFPS_CharacterAnim : public UAnimInstance
{
	GENERATED_BODY()
private:



public:
	UAFPS_CharacterAnim();

	UPROPERTY(BlueprintReadOnly)
	AAFPS_Character* FPSCharacter;

	/** True if character moving vertically */
	UPROPERTY(BlueprintReadOnly)
	bool VerticalInput;

	/** True if character moving vertically in positive Z direction */
	UPROPERTY(BlueprintReadOnly)
	bool VerticalInputPositive;

	/** True if character moving horizontally (WASD) */
	UPROPERTY(BlueprintReadOnly)
	bool HorizontalInput;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;
	
};
