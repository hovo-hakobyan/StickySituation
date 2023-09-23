// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingScore.h"

AFloatingScore::AFloatingScore()
{					
	PrimaryActorTick.bCanEverTick = true;																				  
																								  
}																												  
																												  
// Sets default values																							  
AFloatingScore::AFloatingScore(FVector startPosition, int score)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	FTransform PlaneTransform = FTransform::Identity;
	FQuat Rotation = FQuat(FRotator(0.f, 80.f, 0.f));
	PlaneTransform.SetRotation(Rotation);
	PlaneTransform.SetScale3D(Size);
	BoxComponent->SetWorldTransform(PlaneTransform);

	RootComponent = BoxComponent;


	InitComponents(startPosition, score);

	BoxComponent->SetWorldLocation(startPosition); 

}

void AFloatingScore::InitComponents(FVector startPosition, int score)
{
//Variable set up
	BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName("BoxComponent"));

	FTransform PlaneTransform = FTransform::Identity;
	FQuat Rotation = FQuat(FRotator(0.f, 0.f, 90.f));
	PlaneTransform.SetScale3D(Size);
	PlaneTransform.SetRotation(Rotation);

	RootComponent = BoxComponent;

	UStaticMesh* PlaneStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));

	// Get the path to the folder containing the material
	FString MaterialFolderPath = "/Game/ArtAssets/UI/Materials/Score_Numbers/";

	// Convert score to a string
	FString ScoreStr = FString::FromInt(score);

	// Get the number of digits in the score
	int32 NumDigits = ScoreStr.Len();
	
	UMaterialInterface* Material;

	// Resize the components array to match the number of digits
	VisibleComponents.SetNum(NumDigits);

	// Get the width of a texture, since they will all be the same size.
	float planeOffset = 15.f;

//Component Set up
	for (int32 i = 0; i < NumDigits; i++)
	{
		//Create a name for the component.
		FString ComponentNameString = FString("StaticMeshComponent") + FString::FromInt(i);
		FName ComponentName = FName(*ComponentNameString);

		//Create a StaticMeshComponent
		VisibleComponents[i] = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), ComponentName);
		VisibleComponents[i]->SetupAttachment(BoxComponent);

		// Set the static mesh for the component
		VisibleComponents[i]->SetStaticMesh(PlaneStaticMesh);

		// Set the transform of the component (position, rotation, scale)
		PlaneTransform = FTransform::Identity;
		PlaneTransform.SetLocation(FVector((planeOffset * i), 0.0f, 0.0f));
		PlaneTransform.SetScale3D(Size);
		VisibleComponents[i]->SetWorldTransform(PlaneTransform);


	//Setting up materials
		  // Get the current digit as a character				
		TCHAR CharDigit = ScoreStr[i];
		
		// Convert the character digit to an integer		
		int32 Digit = FCString::Atoi(&CharDigit);

		//Load the material for the current digit
		FString MaterialName = FString::Printf(TEXT("MI_Score%d"), Digit); //This might be wrong because of wrong pathing.
		FString MaterialPath = MaterialFolderPath + MaterialName;
		//UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialName);

		// Load the material using the LoadObject function
		Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
		// Assign the material to the corresponding mesh component
		if (Material && VisibleComponents.IsValidIndex(i))
		{
			VisibleComponents[i]->SetMaterial(0, Material);
		}

		VisibleComponents[i]->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		VisibleComponents[i]->RegisterComponent();
	}
	BoxComponent->SetWorldLocation(startPosition);
	BoxComponent->AddLocalRotation(Rotation);
}														

// Called when the game starts or when spawned
void AFloatingScore::BeginPlay()
{
	Super::BeginPlay();


	//UE_LOG(LogTemp, Warning, TEXT("In beginPlay."));
}

// Called every frame
void AFloatingScore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("In the tick."));
	MoveUp(DeltaTime);
}

void AFloatingScore::MoveUp(float DeltaTime)
{    
	//UE_LOG(LogTemp, Warning, TEXT("Moving Up."));
	// Get the current location of the component
	FVector CurrentLocation = BoxComponent->GetComponentLocation();

	// Calculate the new location by adding the float speed to the Z-axis
	FVector NewLocation = CurrentLocation + FVector(0.f, 0.f, MoveSpeed * DeltaTime);

	// Set the new location of the component
	BoxComponent->SetWorldLocation(NewLocation);


	alphaValue -= (DeltaTime / TimeToDissapear);

	UE_LOG(LogTemp, Warning, TEXT("Alpha: %f"), alphaValue);
	// Iterate through all visible components
	for (UStaticMeshComponent* Component : VisibleComponents)										   
	{																								   
		// Get the material instance assigned to the static mesh									   
		UMaterialInstanceDynamic* MaterialInstance = Component->CreateDynamicMaterialInstance(0);	   
																									   
		// Set the alpha value of the material														   
		MaterialInstance->SetScalarParameterValue("Alpha", alphaValue);
	}
	

	//If the alpha value is above .1f, return, otherwise delete the entire object.
	if (alphaValue > .05f)
		return;

	Destroy();
}																									   
																									   
