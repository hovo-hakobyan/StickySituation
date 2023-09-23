// Fill out your copyright notice in the Description page of Project Settings.


#include "StickySituationGameInstance.h"
#include "StickySituationGameModeBase.h"

UStickySituationGameInstance::UStickySituationGameInstance()
{
	UE_LOG(LogTemp, Warning, TEXT("Created instance"));
}

void UStickySituationGameInstance::OnGameEnd()
{
	ActiveIngredientNum = 0;
	bIsFirstCookedIngredient = true;
}

