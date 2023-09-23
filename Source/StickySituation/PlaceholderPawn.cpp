// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceholderPawn.h"

// Sets default values
APlaceholderPawn::APlaceholderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlaceholderPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlaceholderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlaceholderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

