// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipeManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Ingredient.h"
#include "StickySituationGameModeBase.h"
#include "Dish.h"
#include "Food.h"
#include "Algo/Rotate.h"


URecipeManager::URecipeManager()
{
	
}


void URecipeManager::Initialize()
{
	if (RecipeTable)
	{
		//Load the info into our local array
		const FString context{ TEXT("Recipe Table context") };
		RecipeTable->GetAllRows(context, recipesArr);

		//Sort the array, so that dishes with most ingredients are first
		//recipesArr.Sort();
		

	}
	Dishes.Reserve(DishClasses.Num());
	for (int i = 0; i < DishClasses.Num(); i++)
	{
		if (DishClasses[i])
		{
			Dishes[i] = Cast<ADish>(DishClasses[i].GetDefaultObject());
		}
	}
}

ADish* URecipeManager::TryGetDish(TArray<AIngredient*>& ingredients)
{
	//Can't create a dish if there are no ingredients
	if (ingredients.IsEmpty())
	{
		return nullptr;
	}

	//Go over all the recipes
	for (const FRecipe* recipe : recipesArr)
	{

		//This dish can't be made
		if (!CanDishBeMade(recipe,ingredients))
		{
			continue;
		}
		FString searchString = recipe->Dish;
		//Write lambda to find the correct dish and return
		ADish** FoundDish = Dishes.FindByPredicate([&searchString](const ADish* Dish)
			{
				return Dish->CleanName.Equals(searchString);
			});

		if (FoundDish)
		{
			return *FoundDish;
		}
	}

	return nullptr;
}

FRecipe* URecipeManager::GetRandomRecipe(int currentRound)
{

	if (currentRound == 0)
	{
		activeDishIndex = FMath::RandRange(0, 1);
	}
	else if (currentRound == 1)
	{
		activeDishIndex = FMath::RandRange(2, 3);
	}
	else
	{
		activeDishIndex = 4;
	}

	if (recipesArr[activeDishIndex])
	{
		CurrentIngredients = recipesArr[activeDishIndex]->Ingredients;
		return recipesArr[activeDishIndex];
	}

	return nullptr;
	
}

FString URecipeManager::GetActiveDishName() const
{
	if (activeDishIndex == -1)
	{
		return FString("No dish ");
	}
	return recipesArr[activeDishIndex]->Dish;
}

FString URecipeManager::GetHighPriorityIngredient()
{
	Algo::Rotate(CurrentIngredients, 1);
	return CurrentIngredients[0];
}



bool URecipeManager::CanDishBeMade(const FRecipe* recipe, TArray<AIngredient*>& ingredients)
{
	bool isCurrentIngredientFound = false;
	//Go over all the ingredients that current dish needs
	for (const FString& recipeIngredient : recipe->Ingredients)
	{
		for (int i = 0; i < ingredients.Num(); i++)
		{
			//If this ingredient is already used, move to the next
			if (ingredients[i]->IsUsed)
			{
				continue;
			}
			//If the current ingredient on table is not the ingredient we are looking for, move to the next one
			if (!recipeIngredient.Equals(ingredients[i]->CleanName,ESearchCase::IgnoreCase))
			{
				continue;
			}

			//We were looking for this ingredient, and it's found
			isCurrentIngredientFound = true;

			//Mark the ingredient used
			ingredients[i]->IsUsed = true;

			//No need to keep checking the rest on the plate, since we found what we needed
			break;
		}
		
		//The first ingredient is not present on the table
		//We don't have to keep looking, this dish can't be made
		if (!isCurrentIngredientFound)
		{
			//Set used properties back to false
			for (int i = 0; i < ingredients.Num(); i++)
			{
				ingredients[i]->IsUsed = false;
			}

			return false;
		}

		//Keep looking for the rest of the ingredients
		isCurrentIngredientFound = false;

	}

	return true;
}


