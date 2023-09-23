// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "FloatingScore.generated.h"

UCLASS()
class STICKYSITUATION_API AFloatingScore : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloatingScore();

	// Sets default values for this actor's properties
	AFloatingScore(FVector startPosition, int score);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* BoxComponent;


	//This Static mesh is for testing purposes.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* SMComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MoveSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float TimeToDissapear = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FVector Size = { .20f, .20f, .20f };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveUp(float DeltaTime);
	void InitComponents(FVector startPosition, int score);

private:
	TArray<UStaticMeshComponent*> VisibleComponents;
	float alphaValue = 1.f;

};
