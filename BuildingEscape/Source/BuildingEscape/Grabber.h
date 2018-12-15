// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class AController;
class UInputComponent;
class UPhysicsHandleComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AController* OwnerController = nullptr;
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	FVector OwnerViewPointLocation;
	FRotator OwnerViewPointRotation;

	//How far ahead of the player can we reach in cm
	UPROPERTY(EditAnywhere)
	float Reach = 100.f;

	//Ray-cast and grab what's in reach
	void Grab();

	//Called when grab is released
	void Release();

	//Find (assumed) attached Physics Handle
	void FindPhysicsHandleComponent();

	//Setup (assumed) attached Input Component
	void SetupInputComponent();

	//Set variables for player's viewpoint to determine where they are looking
	void SetOwnerViewPoint();

	//Get the end point of the player's reach
	FVector GetReachInWorld();

	//Return hit for first ohysics body in reach
	FHitResult GetFirstPhysicsBodyInReach();
	
};
