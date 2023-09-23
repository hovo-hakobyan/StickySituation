// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "ChopstickPawn.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "AudioManager.h"
#include "Engine/EngineTypes.h"

AFood::AFood()
{
	foodColliderType = EFoodColliderType::Box;
	Tags.Add(FName("DynamicObject"));
}

// Sets default values
AFood::AFood( const EFoodColliderType& colliderType)
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	foodColliderType = colliderType;
	CreateAndInitializeComponent();

	// init audio components
	FoodCollisionAudioComponent = CreateDefaultSubobject<UAudioComponent>("FoodCollisionAudioComponent");
	FoodCookingAudioComponent = CreateDefaultSubobject<UAudioComponent>("FoodCookingAudioComponent");

	Tags.Add(FName("DynamicObject"));
	Tags.Add(FName("Food"));
}
void AFood::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AudioManagerClass)
	{
		AudioManager = Cast<UAudioManager>(AudioManagerClass.GetDefaultObject());
	}
}

void AFood::PlayAudioComponent(USoundCue* pSoundCue, UAudioComponent* pAudioComponent)
{
	if (!AudioManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No audio manager found"));
		return;
	}

	if (!pSoundCue)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Sound cue found"));
		return;
	}

	if (!pAudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Audio Component found"));
		return;
	}

	AudioManager->PlayAudio(pSoundCue, pAudioComponent);
}

void AFood::PlayCookingAudioComponent(USoundCue* pSoundCue, UAudioComponent* pAudioComponent, float pCookTime)
{
	if (!AudioManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No audio manager found"));
		return;
	}

	if (!pSoundCue)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Sound cue found"));
		return;
	}

	if (!pAudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Audio Component found"));
		return;
	}

	AudioManager->CookingAudio(pSoundCue, pAudioComponent, pCookTime);
}

void AFood::StopAudioComponent(UAudioComponent* pAudioComponent)
{
	if (!AudioManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No audio manager found"));
		return;
	}

	if (!pAudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Audio Component found"));
		return;
	}

	AudioManager->StopAudio(pAudioComponent);
}

void AFood::ResizeCollider(int factor)
{

	if (pSphereCollider)
	{
		float currentRad = pSphereCollider->GetUnscaledSphereRadius();
		currentRad += ForgivnessDistance * factor;
		pSphereCollider->SetSphereRadius(currentRad);
		pSphereCollider->UpdateBounds();
	}
	else if (pBoxCollider)
	{
		FVector current = pBoxCollider->GetUnscaledBoxExtent();
		current += FVector{ ForgivnessDistance * factor,ForgivnessDistance * factor,ForgivnessDistance * factor };
		pBoxCollider->SetBoxExtent(current);
		pBoxCollider->UpdateBounds();

	}
	else if (pCapsuleCollider)
	{
		float currentRad = pCapsuleCollider->GetUnscaledCapsuleRadius();
		float currentHalfHeight = pCapsuleCollider->GetUnscaledCapsuleHalfHeight();
		currentRad += ForgivnessDistance * factor;
		currentHalfHeight += ForgivnessDistance * factor;
		pCapsuleCollider->SetCapsuleSize(currentRad, currentHalfHeight, true);
		pCapsuleCollider->UpdateBounds();
	}


	//if we grew the collider
	if (FMath::Sign(factor) == 1)
	{
		bIsColliderScaled = true;
	}
	else if (FMath::Sign(factor) == -1) //if we shrinked the collider
	{
		bIsColliderScaled = false;
	}
	
}

void AFood::MoveOnBelt(float deltaTime)
{
	Move(deltaTime);
}

void AFood::BeginPlay()
{
	Super::BeginPlay();


	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFood::OnBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AFood::OnEndOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFood::OnComponentHit);

	FName ProfileName = CollisionComponent->GetCollisionProfileName();
	
	if (PhysicalMaterial)
	{
		CollisionComponent->SetPhysMaterialOverride(PhysicalMaterial);
	}

}


void AFood::CreateAndInitializeComponent()
{
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	switch (foodColliderType)
	{
	case EFoodColliderType::Sphere:
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("UShapeComponent"));
		pSphereCollider = Cast<USphereComponent>(CollisionComponent);
		break;
	case EFoodColliderType::Box:
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("UShapeComponent"));
		pBoxCollider = Cast<UBoxComponent>(CollisionComponent);
		break;
	case EFoodColliderType::Capsule:
		CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("UShapeComponent"));
		pCapsuleCollider = Cast<UCapsuleComponent>(CollisionComponent);
		break;
	}
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		CollisionComponent->SetMassOverrideInKg(NAME_None, 1000000.0f);
	}
	
	VisibleComponent->SetGenerateOverlapEvents(false);


	RootComponent = CollisionComponent;
	VisibleComponent->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned

// Called every frame
void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CollisionComponent)
	{
		return;
	}

	HandlePickedUp();
	HandlePhysics();
	Move(DeltaTime);
}


void AFood::HandlePhysics()
{
	CollisionComponent->SetSimulatePhysics(true);

	if (bIsPickedUp)
	{
		CollisionComponent->SetSimulatePhysics(false);
		
		//Grow the collider
		if (!bIsColliderScaled)
		{
			ResizeCollider(1);
		}
	}
	else
	{
		//shring the collider
		if (bIsColliderScaled)
		{
			ResizeCollider(-1);
		}
	}

	FVector ActorLocation = CollisionComponent->GetOwner()->GetActorLocation();
	if (ActorLocation.Z < minHeightPos && !((ActorLocation.X > 205 && ActorLocation.X < 260) && (ActorLocation.Y > -5 && ActorLocation.Y < 32)))
	{
		ActorLocation.Z = minHeightPos;
		CollisionComponent->SetWorldLocation(ActorLocation);
	}
}

void AFood::HandlePickedUp()
{
	bIsPickedUp = false;

	//Check if the amount of chopsticks touching is enough
	if (chopsticksTouching.Num() >= 2)
	{
		bIsPickedUp = true;

		for (AChopstickPawn* chopstick : chopsticksTouching)
		{
			chopstick->bIsHoldingFood = true;
		}
	}
	
	
}

void AFood::Move(float deltaTime)
{
	if (bIsPickedUp)
	{
		//Get the first two elements their location.
		//This won't work if 3 are touching and one of the first two would leave since it only check the first two!
		//Adjust this later.
		if (!chopsticksTouching[0] || !chopsticksTouching[1])
		{
			return;
		}
		auto position1 = chopsticksTouching[0]->GetActorLocation();
		auto position2 = chopsticksTouching[1]->GetActorLocation();

		//Lerp the position the food has to be in at the halfway point between the 2 chopsticks
		foodHeldPosition = FMath::Lerp(position1, position2, 0.5f);

		//Set the parent root node of it as the location.
		CollisionComponent->SetWorldLocation(foodHeldPosition);
	}
	else if (bIsOnBelt)
	{
		//Get Negative right vector
		FVector forwardVector = -FVector::ForwardVector;
		FVector current = CollisionComponent->GetComponentVelocity();
		//UE_LOG(LogTemp, Warning, TEXT("MovementSpeed: %f"), MoveSpeed);
		//UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), deltaTime);
		//UE_LOG(LogTemp, Warning, TEXT("Vector: %s"), *rightVector.ToString());
		FVector moveDirection = FVector{ forwardVector.X * MoveSpeed,0.0f,current.Z }  ;
		CollisionComponent->SetPhysicsLinearVelocity(moveDirection, false); //deltaTime to make the foodObjects time bound for frame drops.
	}
	
}

void AFood::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	//Cast to chopstick
	auto chopstickActor = Cast<AChopstickPawn>(OtherActor);
	//Check if cast failed
	if (!chopstickActor || chopstickActor->bIsHoldingFood)
		return;
	//If it's a chopstick entering, add it to the list.
	chopsticksTouching.Add(chopstickActor);
}

void AFood::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	//Cast to chopstick
	auto chopstickActor = Cast<AChopstickPawn>(OtherActor);
	//Check if cast failed
	if (!chopstickActor)
		return;
	chopstickActor->bIsHoldingFood = false;
	//If it's a chopstick entering, remove it from the list.
	chopsticksTouching.Remove(chopstickActor);
}

void AFood::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//Cast to chopstick
	auto chopstickActor = Cast<AChopstickPawn>(OtherActor);
	//Check if cast failed
	if (chopstickActor)
		return;

	PlayAudioComponent(FoodCollisionCue, FoodCollisionAudioComponent);
}


