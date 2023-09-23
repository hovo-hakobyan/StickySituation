// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structs.h"
#include "SaveGameHS.generated.h"

/**
 * 
 */
UCLASS()
class STICKYSITUATION_API USaveGameHS : public USaveGame
{
	GENERATED_BODY()
	
public:
	USaveGameHS();

	UPROPERTY(VisibleAnywhere, Category = "HighScoreData")
	FHighScoreData HighScoreData;
private:
	
};
