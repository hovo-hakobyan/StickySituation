// Fill out your copyright notice in the Description page of Project Settings.


#include "ChopstickController.h"
#include "StickySituationGameModeBase.h"
#include "PlaceholderPawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "CameraManager.h"
#include "Components/WidgetInteractionComponent.h"
#include "ChopstickPawn.h"


AChopstickController::AChopstickController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bAutoManageActiveCameraTarget = false;
}

void AChopstickController::EnableMovement()
{
	bMovementEnabled = true;
	
}

void AChopstickController::BeginPlay()
{	
	//Get the game mode
	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());

	if (pGameModeRef)
	{
		//Get the game camera
		AActor* pGameCam = pGameModeRef->GameCam;

		if (pGameCam)
		{
			SetViewTarget(pGameCam);
		}


	}

}

void AChopstickController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SpawnAction", IE_Pressed, this, &AChopstickController::Spawn);

	// Bind the input axis mappings to functions that move the pawn
	InputComponent->BindAxis("MoveForward", this, &AChopstickController::MoveForwardCallback);
	InputComponent->BindAxis("MoveRight", this, &AChopstickController::MoveRightCallback);
	InputComponent->BindAxis("MoveUp", this, &AChopstickController::MoveUpCallback);
	InputComponent->BindAxis("RotateRight", this, &AChopstickController::RotateRightCallback);
	InputComponent->BindAxis("RotateLeft", this, &AChopstickController::RotateLeftCallback);

}

void AChopstickController::Tick(float DeltaTime)
{
	if (pControlledPawn)
	{
		if (pControlledPawn->bIsHoldingFood && CurrentAxisValue >0.0f)
		{
			if (pGameModeRef)
			{
				if (pGameModeRef->AreAllChopsticksStationary())
				{
					//If we are holding food, but haven't moved for AccelerationResetTime, reset the currentAxisValue
					//This way the movement will start from 0, aka they have to accelerate their way to full speed
					if (currentAccTimer >= AccelerationResetTime)
					{
						CurrentAxisValue = 0.0f;
						currentAccTimer = 0.0f;
					}
					else
					{
						currentAccTimer += DeltaTime;
					}

				}
			}			
		}
	}
	
}


void AChopstickController::MoveForwardCallback(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	if (FMath::IsNearlyEqual(axisValue, 0.0f))
	{
		return;
	}

	if (pControlledPawn)
	{
		if (pPawnMovement)
		{
			FVector moveForwardVector;
			
			if (pControlledPawn->bIsHoldingFood)
			{
				if (CurrentAxisValue < 1.0f)
				{
					float dt = GetWorld()->GetDeltaSeconds();
					CurrentAxisValue += AccelerationFactor * dt;
				}
				float fastestAxisValue = pGameModeRef->GetFastestChopstickAxisValue();
				moveForwardVector = axisValue > 0.0f ? FVector(0.f, -fastestAxisValue, 0.0f) : FVector(0.f, fastestAxisValue, 0.0f);
			}
			else
			{
				moveForwardVector = FVector(0.f, -axisValue, 0.f);
				CurrentAxisValue = 0.0f;
			}
			Move(moveForwardVector, pPawnMovement);
		}
	}
}


void AChopstickController::MoveRightCallback(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	if (FMath::IsNearlyEqual(axisValue, 0.0f))
	{
		return;
	}

	if (pControlledPawn)
	{
		if (pPawnMovement)
		{
			FVector moveRightVector;

			if (pControlledPawn->bIsHoldingFood)
			{
				if (CurrentAxisValue < 1.0f)
				{
					float dt = GetWorld()->GetDeltaSeconds();
					CurrentAxisValue += AccelerationFactor * dt;
				}
				float fastestAxisValue = pGameModeRef->GetFastestChopstickAxisValue();
				moveRightVector = axisValue > 0.0f ? FVector(fastestAxisValue, 0.0f, 0.0f) : FVector(-fastestAxisValue, 0.0f, 0.0f);
			}
			else
			{
				moveRightVector = FVector(axisValue, 0.f, 0.f);
				CurrentAxisValue = 0.0f;
			}


			Move(moveRightVector, pPawnMovement);
		}
	}
}

void AChopstickController::MoveUpCallback(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	if (FMath::IsNearlyEqual(axisValue, 0.0f))
	{
		
		return;
	}

	if (pControlledPawn)
	{
		
		if (pPawnMovement)
		{
			FVector moveUpVector;

			if (pControlledPawn->bIsHoldingFood)
			{
				if (CurrentAxisValue < 1.0f)
				{
					float dt = GetWorld()->GetDeltaSeconds();
					CurrentAxisValue += AccelerationFactor * dt;
				}
				float fastestAxisValue = pGameModeRef->GetFastestChopstickAxisValue();
				moveUpVector = axisValue > 0.0f ? FVector(0.f, 0.0f, fastestAxisValue) : FVector(0.f, 0.0f, -fastestAxisValue);
			}
			else
			{
				moveUpVector = FVector(0.0f, 0.f, axisValue);
				CurrentAxisValue = 0.0f;
			}

			Move(moveUpVector, pPawnMovement);
		}
	}
}

void AChopstickController::RotateRightCallback(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	if (FMath::IsNearlyEqual(axisValue, 0.0f))
	{
		return;
	}

	if (!pControlledPawn)
	{
		return;
	}

	FRotator currentRot = pControlledPawn->GetActorRotation();

	if (currentRot.Pitch <= -RotationClampAngleDegrees)
	{
		return;
	}
	

	Rotate(axisValue);
	ResizeCapsule(currentRot);

}

void AChopstickController::RotateLeftCallback(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	if (FMath::IsNearlyEqual(axisValue, 0.0f))
	{
		return;
	}

	if (!pControlledPawn)
	{
		return;
	}

	FRotator currentRot = pControlledPawn->GetActorRotation();

	if (currentRot.Pitch >= RotationClampAngleDegrees)
	{
		return;
	}
	
	Rotate(axisValue);
	ResizeCapsule(currentRot);

}

void AChopstickController::Spawn()
{
	//Create the pawn for the first time
	APlaceholderPawn* pPlaceholderPawn = Cast<APlaceholderPawn>(GetPawn());
	if (pPlaceholderPawn)
	{
		if (pGameModeRef)
		{
			pGameModeRef->Spawn(*this);
			pControlledPawn = Cast<AChopstickPawn>(GetPawn());
			if (pControlledPawn)
			{
				pControlledPawn->CapsuleComponent->SetCapsuleSize(InitialColliderRadius, InitialColliderHalfHeight);
				bIsActive = true;
				pGameModeRef->CameraManager->CameraEndsLookingAtKeyboard.AddDynamic(this, &AChopstickController::EnableMovement);
				pPawnMovement = pControlledPawn->GetMovementComponent();
			}
			
		}
	}
	else if(!GetPawn())// reuse existing pawn
	{
		if (pGameModeRef)
		{
			pGameModeRef->ActivatePlayer(*this);
			pControlledPawn = Cast<AChopstickPawn>(GetPawn());
			if (pControlledPawn)
			{
				pPawnMovement = pControlledPawn->GetMovementComponent();
				bIsActive = true;
			}
		}
	}

}

void AChopstickController::Move(FVector& moveVec, UPawnMovementComponent* pMovementComponent)
{
	if (!bMovementEnabled)
	{
		return;
	}

	FVector finalMovement = moveVec.GetClampedToMaxSize(1.f);
	if (pControlledPawn->bIsHoldingFood)
	{
		finalMovement *= MoveSpeed / 2.0f;
	}
	else
	{
		finalMovement *= MoveSpeed;
	}
	
	pMovementComponent->AddInputVector(finalMovement);
}

void AChopstickController::Rotate(float axisValue)
{
	if (!bMovementEnabled)
	{
		return;
	}

	FRotator rotator{ axisValue * RotationSpeed * GetWorld()->GetDeltaSeconds(),0.f,0.f };
	pControlledPawn->AddActorWorldRotation(rotator, true);
}

void AChopstickController::ResizeCapsule(const FRotator& currentRot)
{
	float rotationPercentage = FMath::Clamp(FMath::Abs( currentRot.Pitch) / RotationClampAngleDegrees, 0.f, 1.f);

	float newRad = FMath::Lerp(InitialColliderRadius, MaxColliderRadius, rotationPercentage);
	float newHalfHeight = FMath::Lerp(InitialColliderHalfHeight, MaxColliderHalfHeight, rotationPercentage);

	if (pControlledPawn)
	{
		pControlledPawn->CapsuleComponent->SetCapsuleSize(newRad, newHalfHeight);
	}

}
