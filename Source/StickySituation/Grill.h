// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "Ingredient.h"
#include "Grill.generated.h"

class UNiagaraSystem;

UCLASS()
class STICKYSITUATION_API AGrill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrill();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CollisionComponent")
	class UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MeshComponent")
	UStaticMeshComponent* VisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ButtonCollisionComponent")
		class UBoxComponent* ButtonCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ButtonMeshComponent")
		UStaticMeshComponent* ButtonVisibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* GrillOnMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* GrillOffMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* ButtonOnMaterial;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* ButtonOffMaterial;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Cooking")
		float GrillMaxTimer = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	bool bShouldFlash = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
	bool bHasFlashed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking")
		float FlashTimer = 5.0f;


	//AUDIO
	UPROPERTY()
		class UAudioManager* AudioManager;

	//Audio Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* GrillAudioComponent;
	//AudioCues
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* GrillOnCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* GrillOffCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
		TSubclassOf<class UAudioManager> AudioManagerClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void InitialzeAudioComponent(class USoundCue*& pSoundCue, FString pSoundCuePath);
	void PlayAudioComponent(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrillState")
	bool bIsGrillOn = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnBeginGrillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
		void OnEndGrillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
		void OnBeginButtonOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
		void EndGame();
private:
	float grillTimer;
	
	TArray<AIngredient*>  foodToUpdate;
	class AStickySituationGameModeBase* pGameModeRef;

	FVector buttonStartPosition;

	float currentFlashTimer = 0.0f;

	
	
};
