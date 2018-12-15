// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	
	Owner = GetOwner();

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing PressurePlate!"), *(Owner->GetName()))
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Poll the TriggerVolume
	if (PressurePlate && GetTotalMassOfActorsOnPlate() > TriggerMass)
	{
		OnOpen.Broadcast();
	}
	else
	{
		OnClose.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;

	if (!PressurePlate)
	{
		return TotalMass;
	}

	if (PressurePlate)
	{
		// Find all the overlapping actors
		TArray<AActor*> OverlappingActors;
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);

		// Iterate through them adding their masses
		for (const auto* Overlapper : OverlappingActors)
		{
			TotalMass += Overlapper->FindComponentByClass<UPrimitiveComponent>()->GetMass();
			UE_LOG(LogTemp, Warning, TEXT("%s is on PressurePlate"), *(Overlapper->GetName()))
		}
	}
	
	return TotalMass;
}

