// Fill out your copyright notice in the Description page of Project Settings.


#include "Customer.h"
#include "StickySituationGameModeBase.h"
#include "RecipeManager.h"
#include "Components/AudioComponent.h" //needed for audio
#include "Sound/SoundCue.h"

#include "AudioManager.h"

// Sets default values
ACustomer::ACustomer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set up the collision and put it as the root Component (parent of all)
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	//Here used to be static mesh component. It's gone now

	ThoughtBubblePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FoodSpawnIndicator"));
	ThoughtBubblePosition->SetupAttachment(BoxComponent);

	FloatingScorePosition = CreateDefaultSubobject<USceneComponent>(TEXT("ScoreSpawnIndicator"));
	FloatingScorePosition->SetupAttachment(BoxComponent);

	// init audio components
	CustomerIdleAudioComponent = CreateDefaultSubobject<UAudioComponent>("CustomerIdleAudioComponent");
	CustomerEatAudioComponent = CreateDefaultSubobject<UAudioComponent>("CustomerEatAudioComponent");
	GainScoreAudioComponent = CreateDefaultSubobject<UAudioComponent>("GainScoreAudioComponent");
}

// Called when the game starts or when spawned
void ACustomer::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACustomer::OnBeginCustomerMouthOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACustomer::OnEndCustomerMouthOverlap);


	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());

}

void ACustomer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AudioManagerClass)
	{
		AudioManager = Cast<UAudioManager>(AudioManagerClass.GetDefaultObject());
	}
}

// Called every frame
void ACustomer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!pGameModeRef)
	{
		return;
	}

	if (!bGotCam)
	{
		pGameCam = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode())->GameCam;
		cameraStartRotation = pGameCam->GetActorRotation();
		bGotCam = true;
	}


	if (pGameModeRef->CurrentGameState == ECurrentGameState::GameplayState)
	{
	currentBubbleTimer += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("Current bubble timer: %f"), currentBubbleTimer);
		HandleOrder();
		HandleEatingLogic();

		CheckCustomerState(DeltaTime);
	}

}

void ACustomer::EndTheGame()
{
	//Add code that needs to happen when the game ends (i.e. after the 3 rounds)
	pGameModeRef->SaveHighScoreData(totalScore);
	
	//Broadcasts an event to let subscribers know that the game ended
	bHasOrdered = false;
	totalScore = 0;
	currentBubbleTimer = 0.f;

	thoughtBubble->Destroy();
	thoughtBubble = nullptr;
	
}

void ACustomer::HandleOrder()
{
	//If the customer hasn't ordered 3 times alread and the game is in the gameplaystate, then order.
	if (!bHasOrdered && amountOrdered < 3 && currentBubbleTimer > thoughtBubbleAppearDelay)
	{
		auto OrderedDish = pGameModeRef->RecipeManager->GetRandomRecipe(amountOrdered);
		//auto recipeIngredients = orderedDish->Ingredients;

		if (thoughtBubble != nullptr)
		{
			thoughtBubble->Destroy();
			thoughtBubble = nullptr;
		}
		
		//UE_LOG(LogTemp, Warning, TEXT("Position of the ThoughtBubblePosition is: %s"), *ThoughtBubblePosition->GetComponentLocation().ToString());
		thoughtBubble = GetWorld()->SpawnActor<AThoughtBubble>(ThoughtBubblePosition->GetComponentLocation(), ThoughtBubblePosition->GetComponentRotation());
		//To spawn somthing
		if (thoughtBubble)
		{
			thoughtBubble->ThoughtBubbleInitialize(ThoughtBubblePosition->GetComponentLocation(), OrderedDish);
		}
		
		//The customer has ordered the dish and made an appropriate thought bubble.
		bHasOrdered = true;
	}

	if (amountOrdered == 3)
	{
		if (bShouldBroadcastPregameEnd)
		{
			OnGamePreEnd.Broadcast();
			bShouldBroadcastPregameEnd = false;
		}
		
	}
}

void ACustomer::HandleEatingLogic()
{
	for (int i = 0; i < dishList.Num(); ++i)
	{
		PlayAudioComponent(CustomerEatCue, CustomerEatAudioComponent);

		switch (dishList[i]->ReturnQuality())
		{
		case EDishState::Raw:
		case EDishState::Burned:
			currentCustomerState = CustomerState::NotPleasedWithFood;
			break;
		case EDishState::Cooking:
			currentCustomerState = CustomerState::NormalWithFood;
			break;
		case EDishState::Cooked:
			currentCustomerState = CustomerState::PleasedWithFood;
		default:
			break;
		}

		if (floatingScore != nullptr)
		{
			floatingScore = nullptr;
		}

		//UE_LOG(LogTemp, Warning, TEXT("Position of the ThoughtBubblePosition is: %s"), *ThoughtBubblePosition->GetComponentLocation().ToString());
		floatingScore = GetWorld()->SpawnActor<AFloatingScore>(FloatingScorePosition->GetComponentLocation(), FloatingScorePosition->GetComponentRotation());

		floatingScore->InitComponents(FloatingScorePosition->GetComponentLocation(), dishList[i]->GetScore());
		//paly audio everything u gain points
		PlayAudioComponent(GainScoreCue, GainScoreAudioComponent);

		totalScore += dishList[i]->GetScore();
		dishList[i]->Destroy();

		bHasOrdered = false;
		canTransition = false;
		patienceTimer = 0.f;
		++amountOrdered;
	}
}

void ACustomer::PlayAudioComponent(USoundCue* pSoundCue, UAudioComponent* pAudioComponent)
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

void ACustomer::CheckCustomerState(float DeltaTime)
{
	patienceTimer += DeltaTime;

		if (canTransition)
		{
			if (patienceTimer < neutralTimer)
			{
				currentCustomerState = CustomerState::Happy;
			}
			else if (patienceTimer > neutralTimer && patienceTimer < botheredTimer)
			{
				currentCustomerState = CustomerState::Neutral;
			}
			else if (patienceTimer > botheredTimer && patienceTimer < madTimer)
			{
				currentCustomerState = CustomerState::Bothered;
			}
			else if (patienceTimer > madTimer && patienceTimer < angryTimer)
			{
				currentCustomerState = CustomerState::Mad;
			}
			else if (patienceTimer > angryTimer)
			{
				currentCustomerState = CustomerState::Angry;
			}
				canTransition = false;
		}
}

void ACustomer::DoShake_Implemenatation(float DeltaTime)
{				
	shakeTime += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("Value of shakeValue: %f"), shakeValue);
	//UE_LOG(LogTemp, Warning, TEXT("Value of DeltaTime: %f"), DeltaTime);
	shakeValue = shakeValue + (DeltaTime * shakeSpeed);

	if (shakeValue > maxShakeValue || shakeValue < -maxShakeValue) //If shake value is bigger than 2.5 or smaller then -2.5, change direction.
	{
		//UE_LOG(LogTemp, Warning, TEXT("DeltaTime changed"));
		//DeltaTime *= -1.f;
		shakeValue *= -1.f;
	}
	// Check if the shake is active																								
	if (shakeDuration > shakeTime)
	{									
		//Add rotation logic to the camera.
		pGameCam->AddActorWorldRotation({ shakeValue,0,0 });
	}
}


void ACustomer::OnBeginCustomerMouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto foodActor = Cast<ADish>(OtherActor);

	if (!foodActor)
		return;

	dishList.Add(foodActor);
}

void ACustomer::OnEndCustomerMouthOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	auto foodActor = Cast<ADish>(OtherActor);

	if (!foodActor)
		return;

	dishList.Remove(foodActor);
}

