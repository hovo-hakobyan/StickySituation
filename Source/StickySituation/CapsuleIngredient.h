// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ingredient.h"
#include "CapsuleIngredient.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STICKYSITUATION_API ACapsuleIngredient : public AIngredient
{
	GENERATED_BODY()
	
public:
	ACapsuleIngredient();
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
private:

};
