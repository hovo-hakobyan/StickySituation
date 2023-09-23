// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ingredient.h"
#include "PreparingTable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDishPrepare);

class URecipeManager;
UCLASS()
class STICKYSITUATION_API APreparingTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APreparingTable();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* VisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UShapeComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* ButtonVisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UShapeComponent* ButtonCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* SpawnDishComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DishArray")
		TArray< TSubclassOf<class ADish>> availableDishes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
		int shakeSpeed = 200;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnDishPrepare OnDishPrepare;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Flashing")
	void StartFlashing();
	void StartFlashing_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Flashing")
	void StopFlashing();
	void StopFlashing_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flasing")
	bool bHasFlashed = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);


	UFUNCTION()
		void OnButtonBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
private:

	TArray<AIngredient*>  foodToCheck;
	TArray<AIngredient*>  foodToDelete;
	ADish* preparedDish;
	void Shake(float deltaTime);

	bool bHasShakenOnce = true;
	int hasChangedDirectionCounter;
	FVector startPosition;
	float amountMoved;
	class AStickySituationGameModeBase* pGameModeRef;
	class UStickySituationGameInstance* GameInstanceRef;
};
