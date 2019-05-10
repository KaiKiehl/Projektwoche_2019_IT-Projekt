// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputCoreTypes.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "TubeActor.generated.h"

UCLASS()
class PW_2019_ITPROJEKT_API ATubeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	// Variablendeklaration
	UPROPERTY(EditAnywhere, Category = "PLC-Settings")
		UShapeComponent *Root;	// Wurzelvariable
	UPROPERTY(EditAnywhere, Category = "PLC-Settings")
		UStaticMeshComponent *Mesh;	// Meshvariable

	UPROPERTY(EditAnywhere, Category = "PLC-Settings")
		float fLift;	// Strecke der Bewegung

	bool bPressed = false;	// Tastendruckabfrage

	float initXpos = 0.0f;	// Verschiebung in X-Richtung
	EMouseCursor::Type DefaultCursor = EMouseCursor::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLC-Settings")
		float fSpeed = 20.0f;

public:
	ATubeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;	// DELTA TIME = TIME BETWEEN EACH FRAME
	/*
	// Check Mouse Click
	virtual void NotifyActorOnClicked(FKey ButtonPressed);

	// Check Mouseover
	virtual void NotifyActorBeginCursorOver();
	virtual void NotifyActorEndCursorOver();
	*/
#if WITH_EDITOR
public:
	virtual void PostInitProperties();
#endif
};
