// Fill out your copyright notice in the Description page of Project Settings.


#include "PreparingTable.h"
#include "ChopstickPawn.h"
#include "Components/BoxComponent.h"
#include "CubeIngredient.h"
#include "SphereIngredient.h"
#include "Dish.h"
#include "RecipeManager.h"
#include "StickySituationGameModeBase.h"
#include "StickySituationGameInstance.h"

// Sets default values
APreparingTable::APreparingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("UShapeComponent"));
	RootComponent = CollisionComponent;
	
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	VisibleComponent->SetupAttachment(RootComponent);

	ButtonCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ButtonUShapeComponent"));
	ButtonCollisionComponent->SetupAttachment(RootComponent);
	
	ButtonVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonStaticMeshComponent"));
	ButtonVisibleComponent->SetupAttachment(ButtonCollisionComponent);

	SpawnDishComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DishpawnIndicator"));
	SpawnDishComponent->SetupAttachment(RootComponent);
	Tags.Add(FName("Interactable"));


}

void APreparingTable::StartFlashing_Implementation()
{
}

void APreparingTable::StopFlashing_Implementation()
{
}

// Called when the game starts or when spawned
void APreparingTable::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APreparingTable::OnBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &APreparingTable::OnEndOverlap);

	ButtonCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APreparingTable::OnButtonBeginOverlap);

	startPosition = CollisionComponent->GetComponentLocation();

	//Get the game mode
	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());
	GameInstanceRef = Cast<UStickySituationGameInstance>(GetGameInstance());

}

// Called every frame
void APreparingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!pGameModeRef || !GameInstanceRef)
	{
		return;
	}

	if (!bHasShakenOnce)
	{
		Shake(DeltaTime);
	}
}

void APreparingTable::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto foodActor = Cast<AIngredient>(OtherActor);

	if (!foodActor)
		return;

	foodToCheck.Add(foodActor);
	if (bHasFlashed)
	{
		StopFlashing();
		bHasFlashed = false;
	}
}

void APreparingTable::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	auto foodActor = Cast<AIngredient>(OtherActor);

	if (!foodActor)
		return;


	foodToCheck.Remove(foodActor);
}

void APreparingTable::OnButtonBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto player = Cast<AChopstickPawn>(OtherActor);

	if (!player)
		return;

	if (preparedDish)
	{
		preparedDish = nullptr;
	}

	preparedDish = pGameModeRef->RecipeManager->TryGetDish(foodToCheck);

	if (preparedDish)
	{

		//spawn dish
		preparedDish = GetWorld()->SpawnActor<ADish>(preparedDish->GetClass(), SpawnDishComponent->GetComponentLocation(), SpawnDishComponent->GetComponentRotation());
		if (preparedDish)
		{
			OnDishPrepare.Broadcast();
		}
		for (int i = foodToCheck.Num() - 1; i >= 0; i--)
		{
			if (foodToCheck[i]->IsUsed)
			{
				preparedDish->AddIngredient(foodToCheck[i]);
			}
		}
		preparedDish->Initialize();
		
		for (int i = foodToCheck.Num() - 1; i >= 0; i--)
		{
			if (foodToCheck[i]->IsUsed)
			{
				foodToCheck[i]->Destroy();
				if (GameInstanceRef->ActiveIngredientNum > 0)
				{
					--GameInstanceRef->ActiveIngredientNum;
				}
			}
		}
	}
	else
	{
		bHasShakenOnce = false;
	}

}

void APreparingTable::Shake(float deltaTime)
{
	auto movablePosition = CollisionComponent->GetComponentLocation();

	//This is to check how much we've moved already to deduce when to swap directions.
	float movement = (deltaTime * shakeSpeed);																			
	amountMoved = amountMoved + movement;																				
	if (amountMoved > (shakeSpeed/20) || amountMoved < -(shakeSpeed / 20))
	{																													
		movement = -movement;																							
		++hasChangedDirectionCounter;																					
	}																													
																														
	movablePosition.X += movement;																						
	CollisionComponent->SetWorldLocation(movablePosition);																
	auto distance = movablePosition.Distance(startPosition, movablePosition);

	if(hasChangedDirectionCounter >= 2 && (distance < 1.f || amountMoved >((shakeSpeed / 20) * 4)))
	{																													
		CollisionComponent->SetWorldLocation(startPosition);															
		hasChangedDirectionCounter = 0;																					
		bHasShakenOnce = true;
		amountMoved = 0;
	}																													
}																														
																														
																														
																														
																														