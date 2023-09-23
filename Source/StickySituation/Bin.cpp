// Fill out your copyright notice in the Description page of Project Settings.


#include "Bin.h"
#include "StickySituationGameModeBase.h"
#include "StickySituationGameInstance.h"
#include "Food.h"

// Sets default values
ABin::ABin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set up the collision and put it as the root Component (parent of all)
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	//Set up the StaticMesh and attach it to the BoxComponent.
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	VisibleComponent->SetupAttachment(BoxComponent);

}

// Called when the game starts or when spawned
void ABin::BeginPlay()
{
	Super::BeginPlay();
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABin::OnBeginBinOverlap);
	}

	pGameModeRef = Cast<AStickySituationGameModeBase>(GetWorld()->GetAuthGameMode());
	GameInstanceRef = Cast<UStickySituationGameInstance>(GetGameInstance());
}

// Called every frame
void ABin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABin::OnBeginBinOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
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

