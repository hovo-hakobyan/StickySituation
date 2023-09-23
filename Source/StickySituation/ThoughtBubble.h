// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Ingredient.h"
#include "Structs.h"
#include "ThoughtBubble.generated.h"

UCLASS()
class STICKYSITUATION_API AThoughtBubble : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThoughtBubble();

	//AThoughtBubble(FVector startPosition, TArray<AIngredient> ingredients ,float startSize = 1.f);
	//AThoughtBubble(FVector startPosition, FRecipe* recipe,float startSize = 1.f);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* ThoughtBubbleComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Ingredient1BubbleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* PlusBubbleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Ingredient2BubbleComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MovementSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MaxHeight = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MinHeight = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float ImageOffset = 20.f;
	void InitVisibleComponents(FVector startPosition, FRecipe* recipe);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveBubble(float DeltaTime);
	void ThoughtBubbleInitialize(FVector startPosition, FRecipe* recipe, float startSize = 1.f);
private:
	TArray<UStaticMeshComponent*> VisibleComponents;
	FVector OriginPosition;
	FVector StartPosition;
	bool bGottenPosition;
	bool bIsInitialized = false;
	float movement;
	
};
