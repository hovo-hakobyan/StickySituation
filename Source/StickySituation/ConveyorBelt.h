// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ConveyorBelt.generated.h"

class AFood;

UCLASS()
class STICKYSITUATION_API AConveyorBelt : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AConveyorBelt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<AFood*> foodArrayOnBelt;
	float timePassedSinceLastSpawn;
	float timePassedSincePrioritySpawn;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* VisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* SpawnFoodComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* EndPointComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float spawnInterval = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TArray<	TSubclassOf<class AFood>>  foodToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float PriorityIngredientSpawn = 5.0f;

	TArray<bool> bSpawned;

	UFUNCTION()
		void OnBeginConveyorBeltOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
		void OnEndConveyorBeltOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
		void OnBeginDeletionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	void SpawnFood(float deltaTime);
	void HandleFoodMovement(float deltaTime);

private:
	class AStickySituationGameModeBase* pGameModeRef;
	class UStickySituationGameInstance* GameInstanceRef;

	void SpawnPriorityIngredient();

};
