// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeIngredient.h"

ACubeIngredient::ACubeIngredient():
	AIngredient(EFoodColliderType::Box)
{
}

void ACubeIngredient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACubeIngredient::BeginPlay()
{
	Super::BeginPlay();
}
