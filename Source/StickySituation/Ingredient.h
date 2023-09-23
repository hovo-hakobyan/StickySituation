// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Food.h"
#include "Ingredient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodCooking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodCooked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodBurned);


UENUM(BlueprintType)
enum class EFoodState : uint8
{
	Raw,
	Cooking,
	Cooked,
	Burned,

	//To iterate over enum elements
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EFoodState, EFoodState::Count);


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STICKYSITUATION_API AIngredient : public AFood
{
	GENERATED_BODY()

public:
	AIngredient();
	AIngredient(const EFoodColliderType& colliderType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	float RawToCookingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	float CookingToCookedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	float CookedToBurnedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon")
	class UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstance* Material;

	UPROPERTY()
	bool IsUsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		int MinScore = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		int MaxScore = 120;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnFoodCooking FoodIsCookingEvent;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnFoodCooked FoodIsCookedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnFoodBurned FoodIsBurnedEvent;

	UFUNCTION(BlueprintCallable)
	EFoodState GetIngredientState();

	// Start cooking the food
	void StartCooking();
	// Stop cooking the food
	void StopCooking();
	int GetScore() const;
	void SetIsGrillOn(bool isGrillOn) { bIsGrillOn = isGrillOn; }

private:
	void SetIngredientState();
	void InitializeMaxScore();

	//This function checks the current foodstate, and adjusts the ingredientScore accordingly
	void EvaluateScore();
	void MoveToNextCookState();
	void Cook(float rate);
	void LerpMaterial();
	
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	FName MaterialParamNameBurned{ TEXT("Blend Value Burnt") };
	FName MaterialParamNameCooked{ TEXT("Blend Value Cooked") };
	FName MaterialParamNameCooking{ TEXT("Blend Value Cooking") };


protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	EFoodState ingredientState = EFoodState::Raw;
	int ingredientScore;
	int maxIngredientScore;

	bool bShouldCook;
	bool bIsGrillOn;
	bool bWasCooking;

	float currentCookedTime;
	float nextCookStateChangeTime;
	float totalCookTime;

	class UStickySituationGameInstance* GameInstanceRef;

};
