// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereIngredient.h"

ASphereIngredient::ASphereIngredient():
	AIngredient(EFoodColliderType::Sphere)
{
	
}

void ASphereIngredient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASphereIngredient::BeginPlay()
{
	Super::BeginPlay();
}
