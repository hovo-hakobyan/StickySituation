// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"

#include "CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class STICKYSITUATION_API UCS_CameraShake : public UCameraShakeBase
{

	GENERATED_BODY()
public:
	UCS_CameraShake(const FObjectInitializer& ObjectInitializer);
	~UCS_CameraShake();

	virtual void DoShake_Implemenatation(float DeltaTime, FVector& CameraLocation, FRotator& CameraRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ShakeRotation")
	FRotator shakeRotation{10.f,10.f,10.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ShakeDuration")
	float shakeDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ShakeTime")
	float shakeTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ShakeRadius")
	float shakeRadius = 5.f;


};
