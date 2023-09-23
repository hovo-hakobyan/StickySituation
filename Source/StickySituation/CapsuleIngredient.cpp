// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleIngredient.h"

ACapsuleIngredient::ACapsuleIngredient():
	AIngredient(EFoodColliderType::Capsule)
{
}

void ACapsuleIngredient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACapsuleIngredient::BeginPlay()
{
	Super::BeginPlay();
}
