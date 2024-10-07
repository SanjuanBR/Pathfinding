// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PathfindingMovement.h"

#include "Debug/DebugHelper.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UPathfindingMovement::UPathfindingMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPathfindingMovement::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
}

void UPathfindingMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveToLocation();
}

void UPathfindingMovement::MoveThoughPath(const TArray<FVector>& NewPath)
{
	Path = NewPath;
	CurrentPointIndex = 0;
	SetComponentTickEnabled(true);
}

void UPathfindingMovement::MoveToLocation()
{
	if(Path.Num() == 0)
	{
		SetComponentTickEnabled(false);
		return;
	}

	const FVector CurrentPoint = Path[CurrentPointIndex];
	FVector Direction = (CurrentPoint - Character->GetActorLocation());
	Direction = Direction.GetSafeNormal();
	
	Character->AddMovementInput(Direction, 4.0f);
	
	float Distance = FVector::Dist(FVector(CurrentPoint.X,CurrentPoint.Y,Character->GetActorLocation().Z), Character->GetActorLocation());

	if(Distance < 20.0f)
	{
		if(CurrentPointIndex < Path.Num() - 1)
		{
			CurrentPointIndex++;
		}
		else
		{
			SetComponentTickEnabled(false);
		}
	}

	Debug::Print("CurrentPointIndex: " + FString::FromInt(CurrentPointIndex));
}