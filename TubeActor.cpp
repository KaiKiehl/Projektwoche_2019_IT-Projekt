// Fill out your copyright notice in the Description page of Project Settings.


#include "TubeActor.h"

// Sets default values
ATubeActor::ATubeActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void ATubeActor::BeginPlay()
{
	Super::BeginPlay();
	FVector NewLocation = GetActorLocation();
	initXpos = NewLocation.X;
	if (GetWorld())
	{
		APlayerController* MyController = GetWorld()->GetFirstPlayerController();
		if (!MyController)
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR!"));
			return;
		}
		DefaultCursor = MyController->CurrentMouseCursor;
		UE_LOG(LogTemp, Warning, TEXT("OK!"));
	}

}

// Called every frame
void ATubeActor::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();

	//iVz ist die verschiebung in definierter Richtung. Vorzeichen ist die negative oder positive Richtung

	int iVz = 1;
	if (fLift < 0) iVz = -1;

	if (!bPressed)
	{
		//if (fLift >= 0)
		{
			if (NewLocation.X > initXpos - fLift)
			{
				float factor = -fSpeed;
				NewLocation.X += factor * DeltaTime;
				if (NewLocation.X < initXpos - fLift) NewLocation.X = initXpos - fLift;
				SetActorLocation(NewLocation);
			}
		}
		//else
		{
			if (NewLocation.X < initXpos - fLift)
			{
				float factor = fSpeed;
				NewLocation.X += factor * DeltaTime;
				if (NewLocation.X > initXpos - fLift) NewLocation.X = initXpos - fLift;
				SetActorLocation(NewLocation);
			}
		}
	}
	else
	{
		//if (fLift >= 0)
		{
			if (NewLocation.X < initXpos)
			{
				float factor = fSpeed;
				NewLocation.X += factor * DeltaTime;
				if (NewLocation.X > initXpos) NewLocation.X = initXpos;
				SetActorLocation(NewLocation);
			}
		}
		//else
		{
			if (NewLocation.X > initXpos)
			{
				float factor = -fSpeed;
				NewLocation.X += factor * DeltaTime;
				if (NewLocation.X < initXpos) NewLocation.X = initXpos;
				SetActorLocation(NewLocation);
			}
		}
	}
}
/*
void ATubeActor::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	FString Message;
	Message = FString::Printf(TEXT("IN"));
	Message += FString::Printf(TEXT(" ->"));
	Message += this->GetHumanReadableName();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Message);

	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	MyController->CurrentMouseCursor = EMouseCursor::Hand;
}

void ATubeActor::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("OUT")) + this->GetHumanReadableName());

	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	MyController->CurrentMouseCursor = DefaultCursor;

}

void ATubeActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	bPressed = !bPressed;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Var bPressed: %d"), bPressed));
}
*/

#if WITH_EDITOR
void ATubeActor::PostInitProperties()
{
	Super::PostInitProperties();
}
#endif