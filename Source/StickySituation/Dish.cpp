// Fill out your copyright notice in the Description page of Project Settings.


#include "Dish.h"

// Sets default values
ADish::ADish():
	AFood(EFoodColliderType::Sphere) //This implies all dishes will use a sphere capsule collider.
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Initialize();
}

ADish::ADish(const EFoodColliderType& colliderType) :
	AFood(colliderType)
{
	Initialize();
}

// Called when the game starts or when spawned
void ADish::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADish::Initialize()
{
	//When the dish is created it should know it's ingredients and their quality to know it's own.
	//Iterate over every ingredient given by the plank. 
	//UE_LOG(LogTemp, Warning, TEXT("Size of the used ingredients array: %d"), ingredientsUsedToPrepareArray.Num());
	if (ingredientsUsedToPrepareArray.Num() > 0)
	{

		int averageState = 0;
		int dishScore = 0;
		//Somehow at the time of creating the dish, we should already know what ingredients were exactly used.
		for (auto& ingredient : ingredientsUsedToPrepareArray)
		{
			//Increase the variable by the int of the state
			
			//TODO: Check with hovo how we want to check the score, in the dish itself or through pepping table.
			averageState += static_cast<int>(ingredient->GetIngredientState()); 				   
			dishScore += ingredient->GetScore();												   
		}																						   

		//Take the average out of it by dividing it by the number of ingredients used.
		averageState /= ingredientsUsedToPrepareArray.Num();
		//Cast that value back to a state.
		dishState = static_cast<EDishState>(averageState);
		//UE_LOG(LogTemp, Warning, TEXT("Score of dish: %d"), dishScore);
		//Set the total score of the dush as the combined score of every ingredient.
		SetScore(dishScore);

	}
}

// Called every frame
void ADish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Dish score: %d"), totalDishScore);
}

TArray<TSubclassOf<AIngredient>> ADish::GetIngredients() const
{
	return ingredientsNeededToPrepareArray;
}

void ADish::SetScore(int score)
{
	totalDishScore = score;
}

int ADish::GetScore() const
{
	return totalDishScore;
}

void ADish::AddIngredient(AIngredient* ingredient)
{
	ingredientsUsedToPrepareArray.Add(ingredient);
}

EDishState ADish::ReturnQuality() const
{
	return dishState;
}

