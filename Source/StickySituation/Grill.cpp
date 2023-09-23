// Fill out your copyright notice in the Description page of Project Settings.


#include "Grill.h"
#include "Components/BoxComponent.h"
#include "ChopstickPawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h" // add this include statement
#include "Sound/SoundCue.h"       // add to make audio play
#include "Components/AudioComponent.h"
#include "StickySituationGameModeBase.h"
#include "AudioManager.h"

// Sets default values
AGrill::AGrill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;

	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisibleComponent->SetupAttachment(RootComponent);


	ButtonCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ButtonCollision"));
	ButtonCollisionComponent->SetupAttachment(RootComponent);

	ButtonVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonVisibleComponent->SetupAttachment(ButtonCollisionComponent);
	Tags.Add(FName("Interactable"));

	// init audio components
	GrillAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StoveAudioComponent"));
}
void AGrill::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (AudioManagerClass)
	{
		AudioManager = Cast<UAudioManager>(AudioManagerClass.GetDefaultObject());
	}
}


void AGrill::PlayAudioComponent(USoundCue* pSoundCue, UAudioComponent* pAudioComponent)
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
																														   
// Called when the game starts or when spawned																			   
void AGrill::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrill::OnBeginGrillOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AGrill::OnEndGrillOverlap);

	ButtonCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrill::OnBeginButtonOverlap);

	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());
	buttonStartPosition = ButtonVisibleComponent->GetComponentLocation();
	pGameModeRef->OnGameEnd.AddDynamic(this, &AGrill::EndGame);
}

// Called every frame
void AGrill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!VisibleComponent || !ButtonVisibleComponent)
	{
		return;
	}

	for (auto& ingredient : foodToUpdate)
	{
		ingredient->SetIsGrillOn(bIsGrillOn);
	}

	if (bIsGrillOn)
	{
		VisibleComponent->SetMaterial(0, GrillOnMaterial);
		ButtonVisibleComponent->SetMaterial(0, ButtonOnMaterial);
		ButtonVisibleComponent->SetWorldLocation(buttonStartPosition - FVector{0,0,4});
		grillTimer += DeltaTime;
		if (grillTimer > GrillMaxTimer)
		{
			grillTimer = 0.f;
			bIsGrillOn = false;
			VisibleComponent->SetMaterial(0, GrillOffMaterial);
			ButtonVisibleComponent->SetMaterial(0, ButtonOffMaterial);
			ButtonVisibleComponent->SetWorldLocation(buttonStartPosition);

			// grill button off audio
			PlayAudioComponent(GrillOffCue, GrillAudioComponent);
		}
	}

	if (!bShouldFlash && !bHasFlashed)
	{
		if (pGameModeRef->CurrentGameState == ECurrentGameState::GameplayState)
		{
			currentFlashTimer += DeltaTime;
			if (currentFlashTimer >= FlashTimer)
			{
				bShouldFlash = true;
				currentFlashTimer = 0.0f;
			}
		}
	}
	
}

void AGrill::OnBeginGrillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto ingredientActor = Cast<AIngredient>(OtherActor);
	//UE_LOG(LogTemp, Warning, TEXT("MyActor is of class"));
	if (!ingredientActor)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("Food was an ingredient and set on Grill to true"));
	ingredientActor->SetIsOnGrill(true);
	foodToUpdate.Add(ingredientActor);
}

void AGrill::OnEndGrillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{

	auto ingredientActor = Cast<AIngredient>(OtherActor);

	if (!ingredientActor)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("Food was an ingredient and set on Grill to false"));
	ingredientActor->SetIsOnGrill(false);

	foodToUpdate.Remove(ingredientActor);
}

void AGrill::OnBeginButtonOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	auto player = Cast<AChopstickPawn>(OtherActor);

	if (!player)
		return;

	bIsGrillOn = true;
	bHasFlashed = true;
	
	// grill button off audio
	PlayAudioComponent(GrillOnCue, GrillAudioComponent);
}

void AGrill::EndGame()
{
	currentFlashTimer = 0.0f;
	bHasFlashed = false;
	bShouldFlash = false;
}

