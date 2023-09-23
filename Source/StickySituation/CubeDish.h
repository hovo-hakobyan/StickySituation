// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dish.h"
#include "CubeDish.generated.h"

/**
 * 
 */
UCLASS()
class STICKYSITUATION_API ACubeDish : public ADish
{
	GENERATED_BODY()

public:
	ACubeDish();
protected:
	virtual void Tick(float deltaTime) override;
	virtual void BeginPlay() override;
private:
	
};
