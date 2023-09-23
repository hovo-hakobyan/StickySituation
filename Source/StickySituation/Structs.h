// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/DataTable.h"
#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FRecipe : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Dish;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FString> Ingredients;

	FORCEINLINE bool operator<(const FRecipe& other) const
	{
		//We consider a recipe "bigger" than another one if it has more ingredients

		if (Ingredients.Num() < other.Ingredients.Num())
		{
			return false;
		}
		return true;
	}
};

USTRUCT(BlueprintType)
struct FHighScoreData
{
	GENERATED_BODY();

	FHighScoreData() {}
	FHighScoreData(int score, FString name) 
	{
		HighScore = score;
		PlayerName = name;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int HighScore;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PlayerName;
};