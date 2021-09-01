// Fill out your copyright notice in the Description page of Project Settings.


#include "AFPS_Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

// todo: make input
// todo: fly!!!

AAFPS_Character::AAFPS_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(0, 0, BaseEyeHeight)); // Position the camera
	CameraComp->bUsePawnControlRotation = true;

	// allow fly
	auto& NavAgentProps = GetCharacterMovement()->GetNavAgentPropertiesRef();
	NavAgentProps.bCanFly = true;

	// Create first person mesh
	Mesh1PComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	Mesh1PComp->SetupAttachment(CameraComp);
	Mesh1PComp->bCastDynamicShadow = false;
	Mesh1PComp->CastShadow = false;
	Mesh1PComp->SetRelativeRotation(FRotator(2.f, -15.0f, 5.0f));
	Mesh1PComp->SetRelativeLocation(FVector(0, 0, -155.0f));

	// find default mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms"));
	if (MeshFinder.Succeeded())
	{
		Mesh1PComp->SetSkeletalMesh(MeshFinder.Object);

	}

	// find anim bp
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimFinder(TEXT("/Game/Blueprints/ABP_Player.ABP_Player"));
	if (MeshFinder.Succeeded())
	{
		Mesh1PComp->SetAnimInstanceClass(AnimFinder.Object->GeneratedClass);
	}
}

void AAFPS_Character::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AAFPS_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		DrawDebug();
	#endif
}

void AAFPS_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("FlyForward", this, &AAFPS_Character::FlyForward);
	PlayerInputComponent->BindAxis("FlyRight", this, &AAFPS_Character::FlyRight);
	PlayerInputComponent->BindAxis("FlyUp", this, &AAFPS_Character::FlyUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AAFPS_Character::FlyForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		const FRotator Rotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AAFPS_Character::FlyRight(float Val)
{
	if (Controller && Val != 0.f)
	{
		const FRotator Rotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void AAFPS_Character::FlyUp(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorUpVector(), Val);
	}
}

FORCEINLINE void AAFPS_Character::DrawDebug()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, -1.f, FColor::Cyan, "MovementMode: " + GetCharacterMovement()->GetMovementName());
}
