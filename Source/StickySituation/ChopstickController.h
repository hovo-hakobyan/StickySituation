// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChopstickController.generated.h"

/**
 * 
 */
UCLASS()
class STICKYSITUATION_API AChopstickController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AChopstickController(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationClampAngleDegrees = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicCollision")
	float MaxColliderRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicCollision")
	float MaxColliderHalfHeight = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicCollision")
	float InitialColliderRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicCollision")
	float InitialColliderHalfHeight = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMovementEnabled = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement");
	float AccelerationFactor = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement");
	float AccelerationResetTime = 2.0f;
	
	
	//used to accelerate the chopsticks gradually when holding food
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Movement")
	float CurrentAxisValue = 0.0f;
	UFUNCTION()
	void EnableMovement();




private:

	class AChopstickPawn* pControlledPawn;
	class UPawnMovementComponent* pPawnMovement;
	class AStickySituationGameModeBase* pGameModeRef;
	bool bIsActive = false;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;

	void MoveForwardCallback(float axisValue);
	void MoveRightCallback(float axisValue);
	void MoveUpCallback(float axisValue);
	void RotateRightCallback(float axisValue);
	void RotateLeftCallback(float axisValue);
	void Move(FVector& moveVec, UPawnMovementComponent* pMovementComponent);
	void Rotate(float axisValue);

	void Spawn();

	void ResizeCapsule(const FRotator& currentRot);

	
	float currentAccTimer = 0.0f;
};
