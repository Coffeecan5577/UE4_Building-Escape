// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include <string>

#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UOpenDoor::OpenDoor()
{
	// Set the door rotation.
	//Owner->SetActorRotation(FRotator(0.f, OpenAngle, 0.f));
	OnOpenRequest.Broadcast();
}

void UOpenDoor::CloseDoor()
{
	// Set the door rotation.
	Owner->SetActorRotation(FRotator(0.f, 0.f, 0.f));
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate. "), *GetOwner()->GetName());
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	// Poll the Trigger volume every frame
	if (GetTotalMassOfActorsOnPlate() > 30.0f) // TODO make into a parameter.
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	
	// Check if it's time to close the door.
	if (GetWorld()->GetTimeSeconds() -LastDoorOpenTime > DoorCloseDelay)
	{
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.0f;

	// Find all of the overlapping actors.
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Iterate through them adding their masses.
	for (auto* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());
	}

	return TotalMass;
}
