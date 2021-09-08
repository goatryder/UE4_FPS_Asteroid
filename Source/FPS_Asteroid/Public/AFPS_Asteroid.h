// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFPS_Asteroid.generated.h"

UCLASS()
class FPS_ASTEROID_API AAFPS_Asteroid : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Components, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

public:	
	// Sets default values for this actor's properties
	AAFPS_Asteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// todo: on damage received

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
