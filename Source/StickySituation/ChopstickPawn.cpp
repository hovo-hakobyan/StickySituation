// Fill out your copyright notice in the Description page of Project Settings.


#include "ChopstickPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/WidgetInteractionComponent.h"

// Sets default values
AChopstickPawn::AChopstickPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create root component
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	//Setup visibleComponent and attach to RootComponent
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	VisibleComponent->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Widget Interaction Component"));
	WidgetInteractionComponent->SetupAttachment(VisibleComponent);

	Tags.Add(FName("DynamicObject"));
}

// Called when the game starts or when spawned
void AChopstickPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AChopstickPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AChopstickPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UPawnMovementComponent* AChopstickPawn::GetMovementComponent() const
{
	return MovementComponent;
}

