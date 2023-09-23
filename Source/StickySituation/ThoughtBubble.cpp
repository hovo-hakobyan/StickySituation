// Fill out your copyright notice in the Description page of Project Settings.


#include "ThoughtBubble.h"

// Sets default values
AThoughtBubble::AThoughtBubble()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//AThoughtBubble::AThoughtBubble(FVector startPosition, FRecipe* recipe, float startSize)
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//}

void AThoughtBubble::ThoughtBubbleInitialize(FVector startPosition, FRecipe* recipe, float startSize)
{
	//BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(),FName("BoxComponent"));
	RootComponent = BoxComponent;
	RootComponent->SetWorldLocation(startPosition);
	//This is to make space for the ingredients and the '+' icons in between each one.
	//2 Ingredients will need 1, 3 Ingredients will need 2, 4 ingredients will need 3, etc..
	VisibleComponents.SetNum(recipe->Ingredients.Num() + recipe->Ingredients.Num() - 1);

	InitVisibleComponents(BoxComponent->GetComponentLocation(), recipe);

	//UE_LOG(LogTemp, Warning, TEXT("Position of the BoxComponent is: %s"), *BoxComponent->GetComponentLocation().ToString());
	bIsInitialized = true;
}

void AThoughtBubble::InitVisibleComponents(FVector startPosition, FRecipe* recipe)
{
//Set up Variables
	FQuat Rotation = FQuat(FRotator(0, -0, 90));
	float ingredientImageScale = .25f;
	float thoughtBubbleImageScale = 1.25f;
	int hudElementCounter = 0;
	int iSubtract = 1;
	if (VisibleComponents.Num() == 5)
	{
		ingredientImageScale = 0.20f;
		ImageOffset = 20.f;
		iSubtract = 2;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Size of ingredient visible component is: %d"), VisibleComponents.Num());
	FString MaterialName;
	FString MaterialPath;
	UMaterialInterface* Material;

	// Get the path to the folder containing the material
	FString MaterialFolderPath = "/Game/ArtAssets/UI/Materials/";


	// Load a plane static mesh from disk
	UStaticMesh* PlaneStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	FTransform PlaneTransform = FTransform::Identity;
	PlaneTransform.SetLocation(FVector(0.f, 0.0f, 0.f));
	PlaneTransform.SetScale3D(FVector(thoughtBubbleImageScale, thoughtBubbleImageScale, 1.0f));
	PlaneTransform.SetRotation(Rotation);

//Component set up

	//Set up the thought bubble itself.
	Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/ArtAssets/UI/Materials/MI_ThoughtBubble_UI.MI_ThoughtBubble_UI")));

	ThoughtBubbleComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(),FName("StaticMeshComponent"));
	ThoughtBubbleComponent->SetStaticMesh(PlaneStaticMesh);					  
	ThoughtBubbleComponent->SetWorldTransform(PlaneTransform);				  
	ThoughtBubbleComponent->SetMaterial(0, Material);
	ThoughtBubbleComponent->SetupAttachment(RootComponent);				
	ThoughtBubbleComponent->RegisterComponent();

	for (int i = 0; i < VisibleComponents.Num(); ++i)
	{
		//Create a name for the component.
		FString ComponentNameString = FString("StaticMeshComponent") + FString::FromInt(i);
		FName ComponentName = FName(*ComponentNameString);
		//Create a StaticMeshComponent
		VisibleComponents[i] = NewObject<UStaticMeshComponent>(this, ComponentName);

		// Set the static mesh for the component
		VisibleComponents[i]->SetStaticMesh(PlaneStaticMesh);


		// Set the transform of the component (position, rotation, scale)
		PlaneTransform.SetLocation(FVector((ImageOffset * (i - iSubtract)), 2.f, -1.f));
		PlaneTransform.SetScale3D(FVector(ingredientImageScale, ingredientImageScale, 1.0f));
		PlaneTransform.SetRotation(Rotation);
		VisibleComponents[i]->SetWorldTransform(PlaneTransform);

		//Since thought bubble is it's own component, the first element of the array will be an ingredient.
		//The second element (1), will be a plus.
		if (i % 2 == 0)
		{
			//Get Material for ingredient Icon

			// Get the name of the material you want to load
			MaterialName = "MI_";
			MaterialName += recipe->Ingredients[hudElementCounter];
			MaterialName += "_UI";


			// Construct the full path to the material
			MaterialPath = MaterialFolderPath + MaterialName;

			// Load the material using the LoadObject function
			Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
			// Set the material for the static mesh component
			VisibleComponents[i]->SetMaterial(0, Material);
			++hudElementCounter;
		}
		else
		{
			MaterialName = "MI_Plus_UI";

			MaterialPath = MaterialFolderPath + MaterialName;

			Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));

			//Get Material for '+' icon.
			VisibleComponents[i]->SetMaterial(0, Material);
		}

		VisibleComponents[i]->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		VisibleComponents[i]->RegisterComponent();

		//VisibleComponents[i]->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AThoughtBubble::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AThoughtBubble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bGottenPosition && bIsInitialized)
	{
		OriginPosition = BoxComponent->GetComponentLocation();
		bGottenPosition = true;
	}
	MoveBubble(DeltaTime);
}

void AThoughtBubble::MoveBubble(float DeltaTime)
{
	if (bIsInitialized)
	{
	auto boxCompPos = BoxComponent->GetComponentLocation();

	//This is to check how much we've moved already to deduce when to swap directions.
	if (boxCompPos.Z > (OriginPosition.Z + MaxHeight))
	{
		boxCompPos.Z -= 0.1f;
		MovementSpeed *= -1.f;
	}
	if (boxCompPos.Z < (OriginPosition.Z - MinHeight))
	{
		boxCompPos.Z += 0.1f;
		MovementSpeed *= -1.f;
	}
	movement = (DeltaTime * MovementSpeed);

	boxCompPos.Z += movement;

	//UE_LOG(LogTemp, Warning, TEXT("The current position is: %s"), *boxCompPos.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Position being higher than Orignal pos + MaxHeight is: %s"), (boxCompPos.Z > (OriginPosition.Z + MaxHeight)) ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("Position being higher than Orignal pos - MinHeight is: %s"), (boxCompPos.Z < (OriginPosition.Z - MinHeight)) ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("The Origin is: %s"), *OriginPosition.ToString());
	BoxComponent->SetWorldLocation(boxCompPos);

	}
}

