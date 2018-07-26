// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabberComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabberComponent::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Pressed."));

	// Line trace and see if we reach any actors with physics body collision channel set.
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	/// If we hit something then attach a physics handle
	if (ActorHit)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab, 
			NAME_None, 
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
	
}

void UGrabberComponent::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Key Released."));
	PhysicsHandle->ReleaseComponent();
}

void UGrabberComponent::FindPhysicsHandleComponent()
{
	/// Look for attached PhysicsHandle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		// Physics Handle is found.
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handler component is missing from %s. "), *GetOwner()->GetName());
	}
}

void UGrabberComponent::SetupInputComponent()
{
	/// Look for attached PawnInputComponent (Only appears at runtime.)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		/// Bind input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabberComponent::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabberComponent::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing a pawn input component."), *GetOwner()->GetName());
	}
}


// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player viewpoint this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);

	// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object we're holding.
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
		

}

FHitResult UGrabberComponent::GetFirstPhysicsBodyInReach() const
{
	/// Get player viewpoint this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);

	/// Setup query parameters
	FCollisionQueryParams TraceParameters = (FName(TEXT("")), false, GetOwner());

	/// Line trace (Ray-cast) out to reach distance.
	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(OUT Hit, PlayerViewPointLocation, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);
	

	/// See what we hit.
	AActor* ObjectHit = Hit.GetActor();
	if (ObjectHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has been hit."), *(ObjectHit->GetName()));
	}
	
	return Hit;
}


