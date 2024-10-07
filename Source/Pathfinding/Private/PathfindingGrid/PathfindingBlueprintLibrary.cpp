// Fill out your copyright notice in the Description page of Project Settings.

#include "PathfindingGrid/PathfindingBlueprintLibrary.h"

#include "Debug/DebugHelper.h"
#include "PathfindingGrid/PathfindingGridNode.h"
#include "PathfindingGrid/PathfindingSubsystem.h"
#include "PathfindingGrid/PathfindingVolume.h"

TArray<FVector> UPathfindingBlueprintLibrary::GetPath(const FVector& Start, const FVector& End,	UObject* WorldContextObject)
{
	APathfindingVolume* PathfindingVolume = GetPathfindingVolume(WorldContextObject);
	
	if(PathfindingVolume == nullptr)
	{
		Debug::PrintError("PathfindingVolume is not placed");
		return TArray<FVector>();
	}
	
	APathfindingGridNode* StartNode = PathfindingVolume->GetNearestNode(Start);
	APathfindingGridNode* EndNode = PathfindingVolume->GetNearestNode(End);
	
	TArray<APathfindingGridNode*> PathNodes = PathfindingVolume->FindPath(StartNode, EndNode);
	TArray<FVector> Path;

	for(APathfindingGridNode* Node : PathNodes)
	{
		Path.Add(Node->GetBaseLocation());
	}

	return Path;
}

APathfindingVolume* UPathfindingBlueprintLibrary::GetPathfindingVolume(UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		if (const UPathfindingSubsystem* Subsystem = World->GetSubsystem<UPathfindingSubsystem>())
		{
			return Subsystem->GetPathfindingVolume();
		}
	}
	
	return nullptr;
}
