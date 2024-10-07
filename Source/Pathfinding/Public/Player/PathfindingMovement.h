// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathfindingMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PATHFINDING_API UPathfindingMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPathfindingMovement();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MoveThoughPath(const TArray<FVector>& NewPath);

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	TArray<FVector> Path;

	UPROPERTY()
	int32 CurrentPointIndex = 0;

	UPROPERTY()
	ACharacter* Character;

	void MoveToLocation();

};
