// Fill out your copyright notice in the Description page of Project Settings.


#include "ConveyorBelt.h"
#include "Food.h"
#include "Ingredient.h"
#include "RecipeManager.h"
#include "StickySituationGameModeBase.h"
#include "StickySituationGameInstance.h"

// Sets default values
AConveyorBelt::AConveyorBelt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set up the collision and put it as the root Component (parent of all)
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	//Set up the StaticMesh and attach it to the BoxComponent.
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	VisibleComponent->SetupAttachment(BoxComponent);

	SpawnFoodComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FoodSpawnIndicator"));
	SpawnFoodComponent->SetupAttachment(BoxComponent);

	EndPointComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("FoodDestructionCollider"));
	EndPointComponent->SetupAttachment(BoxComponent);

	bSpawned.Init(false, foodToSpawn.Num());

	Tags.Add(FName("Interactable"));
}

// Called when the game starts or when spawned
void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AConveyorBelt::OnBeginConveyorBeltOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AConveyorBelt::OnEndConveyorBeltOverlap);

	EndPointComponent->OnComponentBeginOverlap.AddDynamic(this, &AConveyorBelt::OnBeginDeletionBoxOverlap);
	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());
	GameInstanceRef = Cast<UStickySituationGameInstance>(GetGameInstance());
}

// Called every frame
void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!pGameModeRef || !GameInstanceRef)
	{
		return;
	}

	if (pGameModeRef->CurrentGameState != ECurrentGameState::GameplayState)
	{
		return;
	}

	if (bSpawned.Num() != foodToSpawn.Num())
	{
		bSpawned.SetNum(foodToSpawn.Num());
		//UE_LOG(LogTemp, Warning, TEXT("bSpawned num: %d"), bSpawned.Num());
	}

	SpawnFood(DeltaTime);
	HandleFoodMovement(DeltaTime);
}

void AConveyorBelt::SpawnFood(float deltaTime)
{
	

	timePassedSinceLastSpawn += deltaTime;
	timePassedSincePrioritySpawn += deltaTime;

	if (timePassedSinceLastSpawn < spawnInterval)
		return;

	if (GameInstanceRef->ActiveIngredientNum >= GameInstanceRef->MaxActiveIngredientNum)
		return;

	if ( timePassedSincePrioritySpawn>= PriorityIngredientSpawn)
	{
		SpawnPriorityIngredient();
		timePassedSinceLastSpawn = 0.0f;
		timePassedSincePrioritySpawn = 0.0f;
		return;
	}

	//Get a random index from the list to spawn.
	int randNum = FMath::RandRange(0, foodToSpawn.Num() - 1);
	
	//Check if the bool of the index is true, if so, return and try to get something next frame.
	if (bSpawned[randNum] == true)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("Idx from Array: %d"), randNum);
	//If false, reset timer because it will spawn
	timePassedSinceLastSpawn = 0;
	//Set index for that object on true for having spawned
	bSpawned[randNum] = true;

	//Spawn object
	auto ingredient = GetWorld()->SpawnActor<AIngredient>(foodToSpawn[randNum], SpawnFoodComponent->GetComponentLocation(), SpawnFoodComponent->GetComponentRotation());
	if (ingredient)
	{
		++GameInstanceRef->ActiveIngredientNum;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Score of inredient is: %d"), ingredient->GetScore());
	//Go through the entire list to see if everything has spawned.
	bool checkEverythingSpawned = true;
	for (auto& isSpawned : bSpawned)
	{
		if (isSpawned == false)
			checkEverythingSpawned = false;
	}

	//If everything has spawned, reset the list to all false, to respawn.
	if (checkEverythingSpawned)
	{
		for (auto& isSpawned : bSpawned)
		{
			isSpawned = false;
		}
	}
}

void AConveyorBelt::HandleFoodMovement(float deltaTime)
{
	for (auto& foodObject : foodArrayOnBelt)
	{
		if (foodObject->GetIsPickedUp())
			continue;


		//Movement speed, and deltaTime used in MoveOnBelt
		foodObject->MoveOnBelt(deltaTime);
	}
}
void AConveyorBelt::SpawnPriorityIngredient()
{
	FString ingredientName = pGameModeRef->RecipeManager->GetHighPriorityIngredient();

	auto FindFoodByCleanName = [&ingredientName](const TSubclassOf<AFood>& FoodClass) {
		// Get the default object of the food class
		AFood* DefaultFood = FoodClass.GetDefaultObject();

		// Check if the CleanName of the default object matches the target CleanName
		return DefaultFood->CleanName.Equals(ingredientName, ESearchCase::IgnoreCase);
	};

	TSubclassOf<AFood>* ingToSpawn = foodToSpawn.FindByPredicate(FindFoodByCleanName);

	if (ingToSpawn)
	{
		auto ing = GetWorld()->SpawnActor<AIngredient>(ingToSpawn->Get(), SpawnFoodComponent->GetComponentLocation(), SpawnFoodComponent->GetComponentRotation());
		if (ing)
		{
			++GameInstanceRef->ActiveIngredientNum;
		}
	}
}
void AConveyorBelt::OnBeginConveyorBeltOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto foodActor = Cast<AFood>(OtherActor);

	if (!foodActor)
		return;

	foodActor->SetIsOnBelt(true);
	foodArrayOnBelt.Add(foodActor);

	//UE_LOG(LogTemp, Warning, TEXT("Amount of food on belt: %d"), static_cast<int>(foodArrayOnBelt.Num()));
}

void AConveyorBelt::OnEndConveyorBeltOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	auto foodActor = Cast<AFood>(OtherActor);

	if (!foodActor)
		return;

	foodActor->SetIsOnBelt(false);
	foodArrayOnBelt.Remove(foodActor);
}

void AConveyorBelt::OnBeginDeletionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto foodActor = Cast<AFood>(OtherActor);

	if (!foodActor)
	return;

	foodActor->Destroy();

	if (GameInstanceRef)
	{
		if (GameInstanceRef->ActiveIngredientNum > 0)
		{
			--GameInstanceRef->ActiveIngredientNum;
		}
	}
	

}


