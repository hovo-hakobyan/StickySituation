// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraMainGameLookEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraKeyboardLookEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraShutterLookEnd);


UCLASS()
class STICKYSITUATION_API ACameraManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCamera")
	AActor* GameCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedTowardsShutters = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedBackToMainGame = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxTravelDistance = 50.0f;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationAngle = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxRiseDistance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TransitionSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	bool bShouldLookAtKeyboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	bool bShouldLookAtMainGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	bool bShouldLookAtShutters;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnCameraShutterLookEnd CameraEndsLookingAtShutters;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnCameraKeyboardLookEnd CameraEndsLookingAtKeyboard;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnCameraMainGameLookEnd CameraEndsLookingAtMainGame;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	

	bool bhasReachedFinalPos;
	bool bhasReachedFinalRot;
	bool bHasReachedFinalHeight;

	FVector initialPosition;
	FRotator initialRotation;

	//Returns true when has finishedLooking
	bool LookAtVirtualKeyboard_Internal(float deltaTime);
	bool LookAtMainGame_Internal(float deltaTime);
	bool LookAtShutters_Internal(float deltaTime);

	void MoveCamera(float deltaTime, const FVector& startPos, const FVector& endPos, float speed);
	void RotateCamera(float deltaTime, const FRotator& startRot, const FRotator& endRot);
	void RiseCamera(float deltaTime, const FVector& startHeight, const FVector& endHeight, float speed);



};
