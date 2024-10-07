// Fill out your copyright notice in the Description page of Project Settings.

#include "PathfindingGrid/PathfindingGridNode.h"
#include "PathfindingGrid/PathfindingSubsystem.h"

APathfindingGridNode::APathfindingGridNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APathfindingGridNode::BeginPlay()
{
	Super::BeginPlay();
}

void APathfindingGridNode::InitializeNode(const FVector& NewNodeSize)
{
	NodeSize = NewNodeSize;

	const FVector NodeLocation = GetActorLocation();
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector3f(NodeSize * 0.5f));

	bool bIsOverlapped = GetWorld()->OverlapAnyTestByObjectType(
		NodeLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllStaticObjects),
		CollisionShape
	);

	bIsWalkable = !bIsOverlapped;
}

TArray<APathfindingGridNode*> APathfindingGridNode::GetNeighbors(const TMap<FGridNodeIndex, APathfindingGridNode*>& GridNodes) const
{
	TArray<APathfindingGridNode*> Neighbors;

	const FGridNodeIndex CurrentIndex = GridNodeIndex;
	
	TArray<FGridNodeIndex> Directions = {
		FGridNodeIndex(CurrentIndex.X + 1, CurrentIndex.Y),
		FGridNodeIndex(CurrentIndex.X - 1, CurrentIndex.Y),
		FGridNodeIndex(CurrentIndex.X, CurrentIndex.Y + 1),
		FGridNodeIndex(CurrentIndex.X, CurrentIndex.Y - 1),
		FGridNodeIndex(CurrentIndex.X + 1, CurrentIndex.Y + 1),
		FGridNodeIndex(CurrentIndex.X - 1, CurrentIndex.Y + 1),
		FGridNodeIndex(CurrentIndex.X + 1, CurrentIndex.Y - 1),
		FGridNodeIndex(CurrentIndex.X - 1, CurrentIndex.Y - 1)
	};

	for (const FGridNodeIndex& Direction : Directions)
	{
		if (GridNodes.Contains(Direction))
		{
			APathfindingGridNode* NeighborNode = *GridNodes.Find(Direction);
			if (NeighborNode && NeighborNode->IsWalkable())
			{
				Neighbors.Add(NeighborNode);
			}
		}
	}

	return Neighbors;
}
