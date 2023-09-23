// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShake.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"


UCS_CameraShake::UCS_CameraShake(const FObjectInitializer& ObjectInitializer) :
    UCameraShakeBase(ObjectInitializer)
{

}

UCS_CameraShake::~UCS_CameraShake()
{
}

void UCS_CameraShake::DoShake_Implemenatation(float DeltaTime, FVector& CameraLocation, FRotator& CameraRotation)
{
    // Check if the shake is active
    if (shakeDuration > 0.f)
    {
        // Calculate the camera shake offset
        FVector ShakeOffset = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f) * shakeRadius;

        // Apply the shake to the camera location
        CameraLocation += ShakeOffset;

        // Set the camera rotation (if desired)shakeRotation
        CameraRotation += FRotator(FMath::FRandRange(-1.f, 1.f) * shakeRotation.Pitch, FMath::FRandRange(-1.f, 1.f) * shakeRotation.Yaw, FMath::FRandRange(-1.f, 1.f) * shakeRotation.Roll);

        // Increment the shake time and duration timer
        shakeTime += DeltaTime;
        shakeDuration -= DeltaTime;
    }
}
