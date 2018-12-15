// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	OwnerController = CastChecked<APawn>(GetOwner())->GetController();
	if (!OwnerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber's OwnerController wasn't set!"))
	}
	
	FindPhysicsHandleComponent();
	SetupInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle)
	{
		return;
	}
	if (PhysicsHandle->GetGrabbedComponent())
	{
		//move the object we're holding
		///Get the location and rotation to set the grabbed component to
		SetOwnerViewPoint();
		FVector ReachInWorld = GetReachInWorld();

		PhysicsHandle->SetTargetLocationAndRotation(ReachInWorld, OwnerViewPointRotation);
	}
	
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab button was pressed!"))

	///LINE TRACE and see if we reach any actors with physics body collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	///If we hit something then attach a physics handle
	if (HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit %s"), *(HitResult.GetActor()->GetName()))

		if (!PhysicsHandle)
		{
			return;
		}

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab button was released!"))

	if (!PhysicsHandle)
	{
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->ReleaseComponent();
	}
}

///Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no Physics Handle!"), *(GetOwner()->GetName()))
	}
}

///Look for attached Input Component (only appears at run-time)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component found on %s."), *(GetOwner()->GetName()))

		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no Input Component!"), *(GetOwner()->GetName()))
	}
}

void UGrabber::SetOwnerViewPoint()
{
	if (!OwnerController)
	{
		return;
	}
	else
	{
		OwnerController->GetPlayerViewPoint(OUT OwnerViewPointLocation, OUT OwnerViewPointRotation);
	}
}

FVector UGrabber::GetReachInWorld()
{
	FVector LineTraceEnd = OwnerViewPointLocation + OwnerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	//Set viewpoint variables for use in LINE TRACE
	SetOwnerViewPoint();

	//Get the end point to LINE TRACE to
	FVector LineTraceEnd = GetReachInWorld();

	///Line Trace (AKA Ray-cast) out to reach distance
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		OwnerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	return Hit;
}

