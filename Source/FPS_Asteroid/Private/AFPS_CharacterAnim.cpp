// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_CharacterAnim.h"

#include "AFPS_Character.h"

UAFPS_CharacterAnim::UAFPS_CharacterAnim()
{

}

void UAFPS_CharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (FPSCharacter)
	{
		VerticalInput = FPSCharacter->GetLastUpInput() != 0.f;
		VerticalInputPositive = FPSCharacter->GetLastUpInput() > 0.f;
		HorizontalInput = FPSCharacter->GetLastForwardInput() != 0.f || FPSCharacter->GetLastRightInput() != 0.f;
	}
}

void UAFPS_CharacterAnim::NativeInitializeAnimation()
{
	FPSCharacter = Cast<AAFPS_Character>(TryGetPawnOwner());
}
