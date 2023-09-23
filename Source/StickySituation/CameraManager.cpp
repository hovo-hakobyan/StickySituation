// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManager.h"
#include "StickySituationGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h" 

// Sets default values
ACameraManager::ACameraManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraManager::BeginPlay()
{
	Super::BeginPlay();
	GameCam = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	if (GameCam)
	{
		initialPosition = GameCam->GetActorLocation();
		initialRotation = GameCam->GetActorRotation();
	}
}

void ACameraManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ACameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GameCam)
	{
		return;
	}

	if (bShouldLookAtShutters)
	{
		if (LookAtShutters_Internal(DeltaTime))
		{
			bShouldLookAtShutters = false;
			bhasReachedFinalPos = false;

			//broadcast that we finished looking at the shutters
			CameraEndsLookingAtShutters.Broadcast();
		}
	}
	else if (bShouldLookAtKeyboard)
	{
		//if we finished looking at the keyboard
		if (LookAtVirtualKeyboard_Internal(DeltaTime))
		{
			bShouldLookAtKeyboard = false;
			bhasReachedFinalRot = false;
			bHasReachedFinalHeight = false;
			bhasReachedFinalPos = false;

			CameraEndsLookingAtKeyboard.Broadcast();
		}
	}
	else if (bShouldLookAtMainGame)
	{
		//If we finished looking at the main game
		if (LookAtMainGame_Internal(DeltaTime))
		{
			bShouldLookAtMainGame = false;
			bhasReachedFinalPos = false;
			bhasReachedFinalRot = false;
			bHasReachedFinalHeight = false;

			CameraEndsLookingAtMainGame.Broadcast();
		}
	}
	
}

void ACameraManager::MoveCamera(float deltaTime, const FVector& startPos, const FVector& endPos, float speed)
{
	//If we have moved enough forward
	if (FMath::IsNearlyEqual(startPos.Y, endPos.Y, TransitionSpeed))
	{
		bhasReachedFinalPos = true;
		return;
	}

	FVector NewPos = FMath::VInterpTo(startPos, endPos, deltaTime, speed);
	if (GameCam)
	{
		GameCam->SetActorLocation(NewPos);
	}
	
}


void ACameraManager::RotateCamera(float deltaTime, const FRotator& startRot, const FRotator& endRot)
{
	//If we have rotated RotationAngle amount
	if (FMath::IsNearlyEqual(startRot.Pitch, endRot.Pitch, TransitionSpeed))
	{
		bhasReachedFinalRot = true;
		return;
	}

	FRotator newRot = FMath::RInterpTo(startRot, endRot, deltaTime, RotationSpeed);
	if (GameCam)
	{
		GameCam->SetActorRotation(newRot);
	}
	

}

void ACameraManager::RiseCamera(float deltaTime, const FVector& startHeight, const FVector& endHeight, float speed)
{

	//If we have moved enough forward
	if (FMath::IsNearlyEqual(startHeight.Z, endHeight.Z, TransitionSpeed))
	{
		bHasReachedFinalHeight = true;
		return;
	}

	FVector NewPos = FMath::VInterpTo(startHeight, endHeight, deltaTime, speed);
	NewPos.Y = startHeight.Y;
	if (GameCam)
	{
		GameCam->SetActorLocation(NewPos);
	}
	
}

bool ACameraManager::LookAtMainGame_Internal(float deltaTime)
{
	//We are looking at the main game	
	if (bHasReachedFinalHeight && bhasReachedFinalRot && bhasReachedFinalPos)
	{
		return true;
	}

	if (!GameCam)
	{
		return false;
	}

	//First we rise down
	if (!bHasReachedFinalHeight)
	{
		FVector currentPos = GameCam->GetActorLocation();
		RiseCamera(deltaTime, currentPos, initialPosition, SpeedBackToMainGame);
	}

	//Then we rotate 
	if (!bhasReachedFinalRot)
	{
		//We are rotating
		FRotator currentRot = GameCam->GetActorRotation();
		RotateCamera(deltaTime, currentRot, initialRotation);
	}

	//Then we move back to look at the game
	if (!bhasReachedFinalPos)
	{
		//We move till we reach MaxTravelDistance
		FVector currentPos = GameCam->GetActorLocation();

		MoveCamera(deltaTime, currentPos, initialPosition, SpeedBackToMainGame);
	}

	return false;
}

bool ACameraManager::LookAtShutters_Internal(float deltaTime)
{
	//If MaxTravelDistance has not been reached yet
	if (!bhasReachedFinalPos)
	{
		//We move till we reach MaxTravelDistance

		FVector forward = GameCam->GetActorForwardVector();
		FVector finalPos = initialPosition + FVector{ 0.0f,FMath::Sign(forward.Y) * MaxTravelDistance,0.0f };
		FVector currentPos = GameCam->GetActorLocation();

		MoveCamera(deltaTime, currentPos, finalPos, SpeedTowardsShutters);
		return false;
	}

	return true;
}

bool ACameraManager::LookAtVirtualKeyboard_Internal(float deltaTime)
{
	if (!bhasReachedFinalPos)
	{
		LookAtShutters_Internal(deltaTime);
	}

	//Finished looking at camera
	if (bhasReachedFinalRot && bHasReachedFinalHeight)
	{
		return true;
	}

	//We rotate till we reach RotationAngle
	if (!bhasReachedFinalRot)
	{
		//We are rotating
		FRotator currentRot = GameCam->GetActorRotation();
		FRotator finalRot = initialRotation + FRotator{ -RotationAngle,0.0f,0.0f };
		RotateCamera(deltaTime,currentRot,finalRot);
	}


	//Rotation is done
	//Now we rise till we reach the MaxRiseDistance
	if (!bHasReachedFinalHeight)
	{
		FVector finalPos = initialPosition + FVector{ 0.0f,0.0f,MaxRiseDistance };
		FVector currentPos = GameCam->GetActorLocation();
		RiseCamera(deltaTime,currentPos,finalPos, SpeedBackToMainGame);
	}


	return false;
}

