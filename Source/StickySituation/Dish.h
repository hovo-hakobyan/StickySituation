// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Food.h"
#include "Ingredient.h"
#include "Dish.generated.h"


UENUM()
enum class EDishState : uint8
{
	Raw,
	Cooking,
	Cooked,
	Burned,

	//To iterate over enum elements
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EDishState, EDishState::Count);

UCLASS()
class STICKYSITUATION_API ADish : public AFood
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADish();
	ADish(const EFoodColliderType& colliderType);
	/// <summary>
	/// This function gets the ingredients needed to make the dish.
	/// </summary>
	/// <returns>Array of Ingredients needed to make the dish</returns>
	TArray<TSubclassOf<AIngredient>> GetIngredients() const;

	/// <summary>
	/// Sets the score of the dish to the score of the parameter.
	/// </summary>
	/// <param name="score"></param>
	void SetScore(int score);

	int GetScore() const;

	void AddIngredient(AIngredient* ingredient);

	/// <summary>
	/// Goes over the state of the ingredients used to make it and returns the average value.
	/// </summary>
	/// <returns>State of the dish.</returns>
	EDishState ReturnQuality() const;
	void Initialize();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingredients");
	TArray<TSubclassOf<AIngredient>> ingredientsNeededToPrepareArray;

	//This is to be filled in when the dish gets created to get the properties and values out of each object.
	TArray<AIngredient*> ingredientsUsedToPrepareArray;
	int totalDishScore = 0;
	EDishState dishState;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
