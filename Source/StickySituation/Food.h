// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Food.generated.h"

class AChopstickPawn;

//UENUM()
//enum class EFoodState : uint8
//{
//	Raw,
//	Cooking,
//	Cooked,
//	Burned,
//
//	//To iterate over enum elements
//	Count UMETA(Hidden)
//};
//ENUM_RANGE_BY_COUNT(EFoodState, EFoodState::Count);


UENUM()
enum class EFoodColliderType : uint8
{
	Sphere,
	Box,
	Capsule
};

UCLASS()
class STICKYSITUATION_API AFood : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFood();
	AFood(const EFoodColliderType& colliderType);

	bool GetIsPickedUp() { return bIsPickedUp; }
	bool GetIsOnGrill() { return bIsOnGrill; }
	bool GetIsOnBelt() { return bIsOnBelt; }

	void SetIsPickedUp(bool isPickedUp) { bIsPickedUp = isPickedUp; }
	void SetIsOnGrill(bool isOnGrill) { bIsOnGrill = isOnGrill;}
	void SetIsOnBelt(bool isOnBelt) { bIsOnBelt = isOnBelt; }

	void MoveOnBelt(float deltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Default Variables")
	FString CleanName;

	//Every food object will have a static mesh, but not every ingredient/dish will have the same collision box, 
	//hence no inheritable collision component.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MeshComponent")
	UStaticMeshComponent* VisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CollisionComponent")
	UShapeComponent* CollisionComponent;

	//audio
	UPROPERTY()
	class UAudioManager* AudioManager;

	// audio components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* FoodCollisionAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* FoodCookingAudioComponent;
	// sound cues
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* FoodCollisionCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* CookingCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
		TSubclassOf<class UAudioManager> AudioManagerClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionComponent")
	float ForgivnessDistance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
	class USphereComponent* pSphereCollider = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
	class UBoxComponent* pBoxCollider = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
	class UCapsuleComponent* pCapsuleCollider = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	void PlayAudioComponent(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent);
	void PlayCookingAudioComponent(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent,float pCookTime);
	void StopAudioComponent(UAudioComponent* pAudioComponent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MoveSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float minHeightPos = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	class UPhysicalMaterial* PhysicalMaterial;

	void HandlePhysics();
	void HandlePickedUp();
	void Move(float deltaTime);
	virtual void CreateAndInitializeComponent();
	
	bool bIsPickedUp = false;
	bool bIsOnGrill = false;
	bool bIsOnBelt = false;
	TArray<AChopstickPawn*> chopsticksTouching;
	FVector foodHeldPosition;
	EFoodColliderType foodColliderType;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void ResizeCollider(int factor);

	

	bool bIsColliderScaled = false;
};
