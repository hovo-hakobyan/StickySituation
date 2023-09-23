// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Structs.h"
#include "RecipeManager.generated.h"

/**
 * 
 */
class AIngredient;
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STICKYSITUATION_API URecipeManager : public UObject
{
	GENERATED_BODY()

public:
	URecipeManager();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Recipe Data Table")
	UDataTable* RecipeTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dishes")
	TArray<TSubclassOf<class ADish>> DishClasses;


	void Initialize();

	/**
	*Looks up a specific dish based on the ingredients, and returns the first dish that can use (some of) the provided ingredients
	* @param ingredients - Ingredients to compare to
	* @return UClass* - If the dish is found, a UClass* will be returned, otherwise nullptr
	*/
	ADish* TryGetDish(TArray<AIngredient*>& ingredients);

	/**
	*Looks up a random dish from the recipe data table,
	* @return FRecipe* - The random recipe
	*/
	FRecipe* GetRandomRecipe(int currentRound);

	FString GetActiveDishName() const;

	FString GetHighPriorityIngredient();

protected:
	
private:
	
	TArray<FRecipe*> recipesArr;

	bool CanDishBeMade(const FRecipe* recipe, TArray<AIngredient*>& ingredients);
	int activeDishIndex = -1;

	UPROPERTY()
	TArray<class ADish*> Dishes;

	TArray<FString> CurrentIngredients;
};
