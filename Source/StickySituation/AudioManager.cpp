// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioManager.h"
#include "Sound/SoundCue.h"


// Sets default values for this component's properties
UAudioManager::UAudioManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAudioManager::PlayAudio_Implementation(USoundCue* pSoundCue, UAudioComponent* pAudioComponent)
{

}

void UAudioManager::CookingAudio_Implementation(USoundCue* pSoundCue, UAudioComponent* pAudioComponent, float pCookingTime)
{

}

void UAudioManager::CustomerAudio_Implementation(USoundCue* pSoundCue, UAudioComponent* pAudioComponent, const CustomerState& pcustomerState)
{
}

void UAudioManager::StopAudio_Implementation(UAudioComponent* pAudioComponent)
{

}


// Called when the game starts
void UAudioManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAudioManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

