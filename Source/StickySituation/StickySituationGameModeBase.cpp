// Copyright Epic Games, Inc. All Rights Reserved.


#include "StickySituationGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Camera/CameraActor.h" 
#include "ChopstickPawn.h"
#include "RecipeManager.h"
#include "SaveGameHS.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CameraManager.h"
#include "ChopstickController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "Ingredient.h"
#include "StickySituationGameInstance.h"
#include "GameplayTagContainer.h"

AStickySituationGameModeBase::AStickySituationGameModeBase()
{
	UE_LOG(LogTemp, Warning, TEXT("Created Mode"));
}


void AStickySituationGameModeBase::BeginPlay()
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();


	Settings->SetVSyncEnabled(true);
	Settings->ApplySettings(true);

	if (RecipeManagerClass)
	{
		RecipeManager = Cast<URecipeManager>(RecipeManagerClass.GetDefaultObject());
		RecipeManager->Initialize();
	}

	if (CameraManagerClass)
	{
		//CameraManager = Cast<ACameraManager>(CameraManagerClass.GetDefaultObject());
		CameraManager = GetWorld()->SpawnActor<ACameraManager>(CameraManagerClass);
		if (CameraManager)
		{
			CameraManager->CameraEndsLookingAtShutters.AddDynamic(this, &AStickySituationGameModeBase::WhenCameraIsZoomedInAtShutters);
			CameraManager->CameraEndsLookingAtMainGame.AddDynamic(this, &AStickySituationGameModeBase::CommenceTheGame);
		}
	}

	//Gets all the player starts and puts it in PlayerStarts array
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	//Get the game camera
	GameCam = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	if (!GameCam)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find game camera"));
	}

	UGameInstance* pGameInstance = GetGameInstance();

	int maxPlayers = 1;
	int nrPlayers = 0;

	if (pGameInstance)
	{
		while (nrPlayers < maxPlayers)
		{
			++nrPlayers;

			FString outError;
			//-1 will take the next available controller and spawn a player on it
			pGameInstance->CreateLocalPlayer(-1, outError, true);
		}

	}

	OnGameEnd.AddDynamic(this, &AStickySituationGameModeBase::OnEndGame);

	USaveGameHS* pSaveGame = Cast<USaveGameHS>(UGameplayStatics::LoadGameFromSlot(SaveGameSlot, 0));
	if (pSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found save file"));
		allTimeHighScoreData = pSaveGame->HighScoreData;

	}
	else
	{	
		pSaveGame = Cast<USaveGameHS>(UGameplayStatics::CreateSaveGameObject(USaveGameHS::StaticClass()));
		if (pSaveGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("Created save file"));
			pSaveGame->HighScoreData.HighScore = 1;
			pSaveGame->HighScoreData.PlayerName = "Devs";

			//actual saving
			UGameplayStatics::SaveGameToSlot(pSaveGame, SaveGameSlot, 0);
			allTimeHighScoreData = pSaveGame->HighScoreData;
		}
	}
}



void AStickySituationGameModeBase::Spawn(APlayerController& playerController)
{
	if (!bEnableSpawning)
	{
		return;
	}

	if (CurrentGameState != ECurrentGameState::SetupState)
	{
		return;
	}
	APawn* pPawn = playerController.GetPawn();
	if (!pPawn)
		return;

	pPawn->Destroy();

	if (PawnToSpawn)
	{
		FTransform spawnLoc = GetNextFreePlayerStart();

		AChopstickPawn* newPawn = GetWorld()->SpawnActor<AChopstickPawn>(PawnToSpawn, spawnLoc);
		newPawn->VisibleComponent->SetMaterial(0,GetNextAvailablePawnMaterial() );
		newPawn->WidgetInteractionComponent->VirtualUserIndex = activePlayersNum;
		newPawn->WidgetInteractionComponent->PointerIndex = activePlayersNum;
		playerController.Possess(newPawn);

		AChopstickController* controller = Cast<AChopstickController>(&playerController);
		if (controller)
		{
			ActivePawnControllers.AddUnique(controller);
			ActivePawns.AddUnique(newPawn);
		}
		++activePlayersNum;
	
		if (GameCam)
		{				
			playerController.SetViewTarget(GameCam);
		}
		
		//when the players are done spawning, look at the keyboard
		if (activePlayersNum == NumPlayersNeededToStart)
		{
			CameraManager->bShouldLookAtKeyboard = true;
			OnAllChopstickSpawn.Broadcast();
		}
	}

	if (!GameInstanceRef)
	{
		GameInstanceRef = Cast<UStickySituationGameInstance>(GetGameInstance());
	}
	
}



float AStickySituationGameModeBase::GetFastestChopstickAxisValue() const
{
	float fastestAxisValue = 0.0f;

	for (const auto& controller : ActivePawnControllers)
	{
		if (controller->CurrentAxisValue > fastestAxisValue)
		{
			fastestAxisValue = controller->CurrentAxisValue;
		}
	}
	return fastestAxisValue;
}

bool AStickySituationGameModeBase::AreAllChopsticksStationary() const
{
	for (const auto& chopstick : ActivePawns)
	{
		const auto& movement = chopstick->GetMovementComponent();

		if (movement)
		{
			if (movement->Velocity.SizeSquared() < FMath::Square(0.05f))
			{
				continue;
			}
		}
		

		//this chopstick is moving, return false
		return false;
	}

	return true;
}

void AStickySituationGameModeBase::SaveHighScoreData(int newScore)
{
	lastHighScoreData = FHighScoreData{newScore,CurrentTeamName};
	//If the score in file is bigger than the new score
	if (allTimeHighScoreData.HighScore >= newScore)
	{
		return;
	}

	USaveGameHS* pSaveGame = Cast<USaveGameHS>(UGameplayStatics::LoadGameFromSlot(SaveGameSlot, 0));
	if (pSaveGame)
	{
		pSaveGame->HighScoreData = lastHighScoreData;

		//actual saving
		UGameplayStatics::SaveGameToSlot(pSaveGame,SaveGameSlot, 0);

		allTimeHighScoreData = lastHighScoreData;
	}
}

const FHighScoreData& AStickySituationGameModeBase::GetLastHighScoreData() 
{
	return lastHighScoreData;
}

bool AStickySituationGameModeBase::ShouldDisplayVirtualKeyboard() const
{

	if (lastHighScoreData.PlayerName.IsEmpty())
	{
		return true;
	}
	return false;
}

void AStickySituationGameModeBase::WhenCameraIsZoomedInAtShutters()
{
	bEnableSpawning = true;
}

void AStickySituationGameModeBase::CommenceTheGame()
{
	CurrentGameState = ECurrentGameState::GameplayState;
}

void AStickySituationGameModeBase::OnEndGame()
{
	CurrentGameState = ECurrentGameState::MenuState;
	DeactivatePlayers();

	CleanUpLevel();
	GameInstanceRef->OnGameEnd();
}

void AStickySituationGameModeBase::BroadcastEndGame()
{
	OnGameEnd.Broadcast();
}

void AStickySituationGameModeBase::AddImpulseToFood()
{
	// Get all actors of AFood class
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFood::StaticClass(), FoundActors);

	// Apply impulse to each food actor
	for (AActor* Actor : FoundActors)
	{
		AFood* FoodActor = Cast<AFood>(Actor);
		if (FoodActor )
		{
			FVector ImpulseDirection = FVector::UpVector; // Define the direction of the impulse
			FVector finalImpulse = FoodImpulse * ImpulseDirection;
			if (FoodActor->pSphereCollider)
			{
				if (FoodActor->pSphereCollider->IsSimulatingPhysics())
				{
					FoodActor->pSphereCollider->AddImpulse(finalImpulse);
				}
				
			}
			else if (FoodActor->pBoxCollider)
			{
				if (FoodActor->pBoxCollider->IsSimulatingPhysics())
				{
					FoodActor->pBoxCollider->AddImpulse(finalImpulse);
				}
				
			}
			else if (FoodActor->pCapsuleCollider)
			{
				if (FoodActor->pCapsuleCollider->IsSimulatingPhysics())
				{
					FoodActor->pCapsuleCollider->AddImpulse(finalImpulse);
				}			
			}
			
		}
	}
}

const FHighScoreData& AStickySituationGameModeBase::GetAllTimeHighScoreData() 
{
	return allTimeHighScoreData;
}

FTransform AStickySituationGameModeBase::GetNextFreePlayerStart()
{
	if (PlayerStarts.Num() <=0 || PlayerStarts.Num() < activePlayersNum)
	{
		return FTransform{};
	}

	if (PlayerStarts[activePlayersNum])
	{
		return PlayerStarts[activePlayersNum]->GetTransform();
	}
	return FTransform{};
}

UMaterialInstance* AStickySituationGameModeBase::GetNextAvailablePawnMaterial()
{
	if (PawnMaterials.Num() < activePlayersNum)
	{
		return nullptr;
	}
	UMaterialInstance* result =PawnMaterials[activePlayersNum];
	if (result)
	{
		return result;
	}
	return nullptr;
}

void AStickySituationGameModeBase::CleanUpLevel()
{
	// Retrieve all actors with the specified tag
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName{ "Food" }, FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		if (FoundActors[i])
		{
			FoundActors[i]->Destroy();
		}
	}

}

void AStickySituationGameModeBase::DeactivatePlayers()
{
	
	for (int i = 0; i < ActivePawnControllers.Num(); i++)
	{
		if (ActivePawnControllers[i])
		{
			APawn* pPawn = ActivePawnControllers[i]->GetPawn();
			if (pPawn)
			{
				ActivePawnControllers[i]->UnPossess();
				FVector location = PlayerStarts[i]->GetTransform().GetTranslation();
				pPawn->SetActorLocation(location);
				pPawn->SetActorHiddenInGame(true);
				pPawn->SetActorEnableCollision(false);
				pPawn->SetActorTickEnabled(false);
				--activePlayersNum;
			}
		}
	}

}

void AStickySituationGameModeBase::ActivatePlayer(APlayerController& playerController)
{
	if (!bEnableSpawning)
	{
		return;
	}

	AChopstickController* controller = Cast<AChopstickController>(&playerController);
	if (controller)
	{
		int idx = ActivePawnControllers.Find(controller);

		if (idx != INDEX_NONE)
		{
			if (ActivePawns[idx])
			{
				controller->Possess(ActivePawns[idx]);	
				ActivePawns[idx]->SetActorHiddenInGame(false);
				ActivePawns[idx]->SetActorEnableCollision(true);
				ActivePawns[idx]->SetActorTickEnabled(true);
				++activePlayersNum;

				//when the players are done spawning, look at the keyboard
				if (activePlayersNum == 2)
				{
					CameraManager->bShouldLookAtKeyboard = true;
					OnAllChopstickSpawn.Broadcast();
				}
			}
			
		}
	}

}
