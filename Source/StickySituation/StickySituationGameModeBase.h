// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structs.h"
#include "StickySituationGameModeBase.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChopstickSpawn);

UENUM()
enum class ECurrentGameState : uint8
{
	MenuState,
	SetupState,
	GameplayState
};


UCLASS()
class STICKYSITUATION_API AStickySituationGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AStickySituationGameModeBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStart")
	TArray<AActor*> PlayerStarts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	TSubclassOf<class APawn> PawnToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCamera")
	AActor* GameCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class ACameraManager> CameraManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	TArray<UMaterialInstance*> PawnMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game state")
		ECurrentGameState CurrentGameState = ECurrentGameState::MenuState;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnGameEnd OnGameEnd;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FOnChopstickSpawn OnAllChopstickSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game state")
	bool bEnableSpawning = false;

	/**
	*Creates a PawnToSpawn specified in the game mode
	* @param playerController - Player controller to possess this pawn 
	*/
	void Spawn(APlayerController& playerController);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TSubclassOf<class URecipeManager> RecipeManagerClass;

	UPROPERTY()
	class URecipeManager* RecipeManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class ACameraManager* CameraManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		FString CurrentTeamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int NumPlayersNeededToStart = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FHighScoreData lastHighScoreData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FHighScoreData allTimeHighScoreData;

	

	float GetFastestChopstickAxisValue() const;
	bool AreAllChopsticksStationary() const;

	/**
	* Checks if highScoreData is bigger than the current High Score Data. If so, it overrides the existing high score, otherwise it does nothign
	* @param highScoreData - the new high score to check and override
	*/
	void SaveHighScoreData(int newScore);

	/**
	 Returns the high score data of the current high score
	*/
	UFUNCTION(BlueprintCallable)
	const FHighScoreData& GetLastHighScoreData();

	UFUNCTION(BLueprintCallable)
	const FHighScoreData& GetAllTimeHighScoreData();

	UFUNCTION(BlueprintCallable)
	bool ShouldDisplayVirtualKeyboard() const;

	UFUNCTION()
	void WhenCameraIsZoomedInAtShutters();

	UFUNCTION()
	void CommenceTheGame();

	UFUNCTION()
	void OnEndGame();

	UFUNCTION(BlueprintCallable)
	void BroadcastEndGame();

	UFUNCTION(BlueprintCallable)
	void AddImpulseToFood();

	void DeactivatePlayers();
	void ActivatePlayer(APlayerController& playerController);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
		float FoodImpulse =  100.0f;
protected:
private:

	FString SaveGameSlot = "DefaultSlot";

	virtual void BeginPlay() override;

	FTransform GetNextFreePlayerStart();
	UMaterialInstance* GetNextAvailablePawnMaterial();
	void CleanUpLevel();

	TArray<class AChopstickController*> ActivePawnControllers;
	TArray<class AChopstickPawn*> ActivePawns;
	class UStickySituationGameInstance* GameInstanceRef;
	

	int activePlayersNum = 0;



};
