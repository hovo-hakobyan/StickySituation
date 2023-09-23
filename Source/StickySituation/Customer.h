// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Dish.h"
#include "FloatingScore.h"
#include "ThoughtBubble.h"
#include "Customer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGamePreEnd);


UENUM(BlueprintType)
enum class CustomerState : uint8
{
	//Idle
	Happy = 0,
	Neutral = 1,
	Bothered =2,
	Angry =3,
	Mad =4,

	//Satisfaction
	PleasedWithFood =5,
	NormalWithFood =6,
	NotPleasedWithFood =7,

	//To iterate over enum elements
	Count UMETA(Hidden)
};


UCLASS()
class STICKYSITUATION_API ACustomer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomer();
	//Comment
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* ThoughtBubblePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* FloatingScorePosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FRotator shakeRotation = {10.f, 10.f, 10.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float shakeDuration = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float shakeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float shakeValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float maxShakeValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int shakeSpeed = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		bool canTransition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float thoughtBubbleAppearDelay= 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomerState")
	CustomerState currentCustomerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PatienceTimer")
		float neutralTimer = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PatienceTimer")
		float botheredTimer= 45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PatienceTimer")
		float madTimer = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PatienceTimer")
		float angryTimer = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PatienceTimer")
	int amountOrdered = 0;
	//audio
	UPROPERTY()
		class UAudioManager* AudioManager;

	//audio components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* CustomerIdleAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* CustomerEatAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* GainScoreAudioComponent;
	//sound cues
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* CustomerIdleCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* CustomerEatCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* GainScoreCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Manager")
		TSubclassOf<class UAudioManager> AudioManagerClass;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
	FGamePreEnd OnGamePreEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bShouldBroadcastPregameEnd = true;

	//Call this function when the game ends
	UFUNCTION(BlueprintCallable)
	void EndTheGame();

	TArray<ADish*> dishList;
	float testTimer = 0.f;

	float patienceTimer;

	FRotator cameraStartRotation;
private:
	bool bGotCam = false;
	bool bHasOrdered = false;

	
	int totalScore = 0;

	float currentBubbleTimer = 0.f;
	
	AActor* pGameCam;
	class AStickySituationGameModeBase* pGameModeRef;
	
	AThoughtBubble* thoughtBubble = nullptr;
	AFloatingScore* floatingScore = nullptr;
	ADish* orderedDish;

	void HandleOrder();
	void HandleEatingLogic();

	void InitialzeAudioComponent(class USoundCue*& pSoundCue, UAudioComponent*& pAudioComponent, FString pSoundCuePath, FString pAudioComponentName);
	void PlayAudioComponent(class USoundCue* pSoundCue, UAudioComponent* pAudioComponent);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void DoShake_Implemenatation(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckCustomerState(float Deltatime);
	//void EvaluateDish();
	

	UFUNCTION()
		void OnBeginCustomerMouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
		void OnEndCustomerMouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
};
