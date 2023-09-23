// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ingredient.h"
#include "CubeIngredient.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STICKYSITUATION_API ACubeIngredient : public AIngredient
{
	GENERATED_BODY()
	
public:
	ACubeIngredient();
private:
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
};
