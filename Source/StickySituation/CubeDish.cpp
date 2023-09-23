// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDish.h"

ACubeDish::ACubeDish():
	ADish(EFoodColliderType::Box)
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACubeDish::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void ACubeDish::BeginPlay()
{
	Super::BeginPlay();
}
