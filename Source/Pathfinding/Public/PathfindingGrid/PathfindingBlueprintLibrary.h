// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathfindingBlueprintLibrary.generated.h"

class APathfindingVolume;


UCLASS()
class PATHFINDING_API UPathfindingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	static TArray<FVector> GetPath(const FVector& Start, const FVector& End, UObject* WorldContextObject); 
	
private:
	static APathfindingVolume* GetPathfindingVolume(UObject* WorldContextObject);
};
