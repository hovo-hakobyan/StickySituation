// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dish.h"
#include "CapsuleDish.generated.h"

/**
 *
 */
UCLASS()
class STICKYSITUATION_API ACapsuleDish : public ADish
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapsuleDish();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
