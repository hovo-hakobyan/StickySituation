// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingredient.h"
#include "Components/SphereComponent.h"
#include "Engine/Texture2D.h"
#include "Components/AudioComponent.h" //needed for audio
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "PreparingTable.h"
#include "StickySituationGameInstance.h"
#include "AudioManager.h"


AIngredient::AIngredient():
	AFood(EFoodColliderType::Sphere)
{
	InitializeMaxScore();
	EvaluateScore();
}

AIngredient::AIngredient(const EFoodColliderType& colliderType):
	AFood(colliderType)
{
	//Default values, should be overriden in the editor
	RawToCookingTime = 1.f;
	CookingToCookedTime = 5.f;
	CookedToBurnedTime = 1.5f;
	totalCookTime = RawToCookingTime + CookingToCookedTime + CookedToBurnedTime;
	InitializeMaxScore();
	EvaluateScore();
}


void AIngredient::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaxScore();
	EvaluateScore();

	currentCookedTime = 0.f; 
	nextCookStateChangeTime = RawToCookingTime;

	if (VisibleComponent && Material)
	{
		VisibleComponent->SetMaterial(0, Material->GetBaseMaterial());

		// Create a dynamic material instance based on the original material instance
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, nullptr);

		// Use the dynamic material instance instead of the original material instance
		VisibleComponent->SetMaterial(0, DynamicMaterialInstance);
	}
	auto gameInstance = GetGameInstance();
	if (gameInstance)
	{
		GameInstanceRef = Cast<UStickySituationGameInstance>(gameInstance);
	}
	
}

void AIngredient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("The boolean 'bIsOnGrill' value is %s"), (bIsOnGrill ? TEXT("true") : TEXT("false")));
	//UE_LOG(LogTemp, Warning, TEXT("The boolean 'bIsGrillOn' value is %s"), (bIsGrillOn ? TEXT("true") : TEXT("false")));
	//UE_LOG(LogTemp, Warning, TEXT("The boolean 'bIsPickedUp' value is % s"), (bIsPickedUp ? TEXT("true") : TEXT("false")));

	if (!GameInstanceRef)
	{
		return;
	}
	if (bIsOnGrill && bIsGrillOn && !bIsPickedUp)
	{
		Cook(DeltaTime);

		PlayCookingAudioComponent(CookingCue, FoodCookingAudioComponent, currentCookedTime);

		bWasCooking = true;
	}
	else if(bWasCooking && (!bIsOnGrill || !bIsGrillOn || bIsPickedUp))
	{
		StopAudioComponent(FoodCookingAudioComponent);

		bWasCooking = false;
	}
}

void AIngredient::EvaluateScore()
{
	switch (ingredientState)
	{
		//When it's raw, we want 20% of max value
	case EFoodState::Raw:
		ingredientScore = maxIngredientScore * 0.2f;
		break;
		//Cooking state gives 70% of the maxvalue
	case EFoodState::Cooking:
		ingredientScore = maxIngredientScore * 0.7f;
		break;
		//When it's properly cooked, we want to give max score
	case EFoodState::Cooked:
		ingredientScore = maxIngredientScore;
		break;
		//When it's burned we want 10% of the maxscore
	case EFoodState::Burned:
		ingredientScore = maxIngredientScore * 0.1f;
		break;

	}
}
void AIngredient::MoveToNextCookState()
{
	switch (ingredientState)
	{
	case EFoodState::Raw:
		ingredientState = EFoodState::Cooking;
		//when should we change food state next time
		nextCookStateChangeTime += CookingToCookedTime;

		FoodIsCookingEvent.Broadcast();

		break;
	case EFoodState::Cooking:
		ingredientState = EFoodState::Cooked;
		//when should we change food state next time
		nextCookStateChangeTime += CookedToBurnedTime;

		FoodIsCookedEvent.Broadcast();

		if (GameInstanceRef->bIsFirstCookedIngredient)
		{
			auto plateActor = UGameplayStatics::GetActorOfClass(GetWorld(), APreparingTable::StaticClass());
			if (plateActor)
			{
				auto plate = Cast<APreparingTable>(plateActor);
				if (plate)
				{
					plate->StartFlashing();
					GameInstanceRef->bIsFirstCookedIngredient = false;
				}
			}
		}
		break;
	case EFoodState::Cooked:
		ingredientState = EFoodState::Burned;
		bShouldCook = false;

		FoodIsBurnedEvent.Broadcast();

		break;
	}
}
EFoodState AIngredient::GetIngredientState()
{
	return ingredientState;
}

void AIngredient::StartCooking()
{
	
	if (static_cast<uint8>(ingredientState) != static_cast<uint8>(EFoodState::Burned) && bIsOnGrill && bIsGrillOn && !bIsPickedUp)
	{
		bShouldCook = true;


		if (!AudioManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("No audio manager found"));
			return;
		}

		if (!CookingCue)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Sound cue found"));
			return;
		}

		if (!FoodCookingAudioComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Audio Component found"));
			return;
		}


		AudioManager->CookingAudio(CookingCue, FoodCookingAudioComponent, currentCookedTime);
	}

}

void AIngredient::StopCooking()
{
	//This gets entered every frame, so stop audio gets called every frame
	
	if (static_cast<uint8>(ingredientState) == static_cast<uint8>(EFoodState::Burned) || !bIsOnGrill || !bIsGrillOn || bIsPickedUp)
	{
		bShouldCook = false;	

		StopAudioComponent(FoodCookingAudioComponent);
	}

}

void AIngredient::Cook(float deltaTime)
{
	currentCookedTime += deltaTime;

	if (currentCookedTime >= nextCookStateChangeTime)
	{
		MoveToNextCookState();
		EvaluateScore();
	}

	// Update the material value
	LerpMaterial();
	
}
void AIngredient::LerpMaterial()
{
	//the formula maps each cooking block (raw to cooking, cooking to cooked, cooked to burned) to [0,1] range
	//This is used inside the material to lerp the texture
	float progress = 0.f;
	switch (ingredientState)
	{
	case EFoodState::Raw:
		progress = currentCookedTime / nextCookStateChangeTime;
		DynamicMaterialInstance->SetScalarParameterValue(MaterialParamNameCooking, progress);
		break;
	case EFoodState::Cooking:
		progress = (currentCookedTime - RawToCookingTime) / (nextCookStateChangeTime - RawToCookingTime);
		DynamicMaterialInstance->SetScalarParameterValue(MaterialParamNameCooked, progress);
		break;
	case EFoodState::Cooked:
		progress = (currentCookedTime - CookingToCookedTime) / (nextCookStateChangeTime - CookingToCookedTime);
		DynamicMaterialInstance->SetScalarParameterValue(MaterialParamNameBurned, progress);
		break;
	}

}

int AIngredient::GetScore() const
{
	return ingredientScore;
}

void AIngredient::SetIngredientState()
{
}

void AIngredient::InitializeMaxScore()
{
	//Each ingredient will get a random max score that can be earned
	//Values might need tweaking
	maxIngredientScore = FMath::RandRange(MinScore, MaxScore);
}
