// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "StickySituationGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STICKYSITUATION_API UStickySituationGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UStickySituationGameInstance();

	UPROPERTY(BlueprintReadWrite, Category = "Cooking")
	bool bIsFirstCookedIngredient = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Food")
		int ActiveIngredientNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
		int MaxActiveIngredientNum = 5;

	void OnGameEnd();
private:

};
