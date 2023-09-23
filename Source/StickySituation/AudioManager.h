// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Customer.h"
#include "Components/ActorComponent.h"
#include "AudioManager.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STICKYSITUATION_API UAudioManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAudioManager();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioFunctions")
	void PlayAudio(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent);
	void PlayAudio_Implementation(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioFunctions")
	void CookingAudio(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent, float pCookingTime);
	void CookingAudio_Implementation(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent, float pCookingTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioFunctions")
	void CustomerAudio(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent, const CustomerState& pcustomerState);
	void CustomerAudio_Implementation(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent, const CustomerState& pcustomerState);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioFunctions")
	void StopAudio(UAudioComponent* pAudioComponent);
	void StopAudio_Implementation(UAudioComponent* pAudioComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
