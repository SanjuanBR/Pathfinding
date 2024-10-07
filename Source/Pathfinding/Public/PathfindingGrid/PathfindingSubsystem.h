// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PathfindingSubsystem.generated.h"

class APathfindingVolume;

UCLASS()
class PATHFINDING_API UPathfindingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SetPathfindingVolume(APathfindingVolume* NewVolume){PathfindingVolume = NewVolume;}
	APathfindingVolume* GetPathfindingVolume() const {return PathfindingVolume;}

private:
	UPROPERTY()
	APathfindingVolume* PathfindingVolume;
};
