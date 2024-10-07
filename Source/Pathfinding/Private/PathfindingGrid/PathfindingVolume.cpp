// Fill out your copyright notice in the Description page of Project Settings.

#include "PathfindingGrid/PathfindingVolume.h"

#include "Components/BoxComponent.h"
#include "Debug/DebugHelper.h"
#include "PathfindingGrid/GridNodeData.h"
#include "PathfindingGrid/PathfindingGridNode.h"
#include "PathfindingGrid/PathfindingSubsystem.h"

APathfindingVolume::APathfindingVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
}

void APathfindingVolume::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
	SetInSubsystem();
}

void APathfindingVolume::SetInSubsystem()
{
	if (const UWorld* World = GetWorld())
	{
		if (UPathfindingSubsystem* Subsystem = World->GetSubsystem<UPathfindingSubsystem>())
		{
			Subsystem->SetPathfindingVolume(this);
		}
	}
}

void APathfindingVolume::InitializeGrid()
{
	GridNodes.Empty();
	
	const FVector Origin = GetActorLocation();
	const FVector BoxExtent = BoxComponent->GetScaledBoxExtent();

	const int32 GridWidth = FMath::CeilToInt(BoxExtent.X * 2 / NodeSize);
	const int32 GridDepth = FMath::CeilToInt(BoxExtent.Y * 2 / NodeSize);

	for (int32 X = 0; X < GridWidth; X++)
	{
		for (int32 Y = 0; Y < GridDepth; Y++)
		{
			const float NodeX = Origin.X + X * NodeSize - BoxExtent.X;
			const float NodeY = Origin.Y + Y * NodeSize - BoxExtent.Y;
			const float NodeZ = Origin.Z - BoxExtent.Z * 0.5f;
			
			FVector NodeLocation = FVector(NodeX, NodeY, NodeZ);
			FTransform NodeTransform(NodeLocation);

			APathfindingGridNode* GridNode = GetWorld()->SpawnActor<APathfindingGridNode>(GridNodeClass, NodeTransform);
			if (GridNode)
			{
				GridNode->InitializeNode(FVector(NodeSize, NodeSize, BoxExtent.Z));
				GridNode->SetCoordinate(FGridNodeIndex(X,Y));
				GridNodes.Add(FGridNodeIndex(X,Y), GridNode);
			}
		}
	}
	
	#if WITH_EDITOR
		if(bDrawDebug)
		{
			TArray<APathfindingGridNode*> GridNodesArray;
			GridNodes.GenerateValueArray(GridNodesArray);
			for(APathfindingGridNode* Node : GridNodesArray)
			{
				FColor BoxColor = Node->IsWalkable() ? FColor::Cyan : FColor::Red;
				FVector BoxLocation = Node->GetBaseLocation();
				DrawDebugBox(GetWorld(), BoxLocation, FVector(Node->GetNodeSize().X * 0.5f,Node->GetNodeSize().Y * 0.5f,1.f), BoxColor, true);
			
				DrawDebugPoint(GetWorld(),BoxLocation,10.f,BoxColor,true);
			
			}
		}
	#endif
}

APathfindingGridNode* APathfindingVolume::GetNearestNode(const FVector& Location) const
{
	APathfindingGridNode* NearestNode = nullptr;
	float MinDistance = FLT_MAX;

	TArray<APathfindingGridNode*> Values;
	GridNodes.GenerateValueArray(Values);
	
	for (APathfindingGridNode* Node : Values)
	{
		float Distance = FVector::Dist(Location, Node->GetActorLocation());

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			NearestNode = Node;
		}
	}

	return NearestNode;
}

void APathfindingVolume::TestNeighbor(FGridNodeIndex NodeIndex, const FColor Color)
{
	if(!GridNodes.Contains(NodeIndex)){return;}
	
	APathfindingGridNode* GridNode = GridNodes[NodeIndex];
	TArray<APathfindingGridNode*> Neighbors = GridNode->GetNeighbors(GridNodes);

	FVector BoxMainLocation = GridNode->GetActorLocation() - FVector(0.0f, 0.0f, GridNode->GetNodeSize().Z * 0.4f);
	DrawDebugPoint(GetWorld(),BoxMainLocation,20.f,Color,true);

	for (APathfindingGridNode* Neighbor : Neighbors)
	{
		FVector BoxLocation = Neighbor->GetActorLocation() - FVector(0.0f, 0.0f, Neighbor->GetNodeSize().Z * 0.4f);
		DrawDebugPoint(GetWorld(),BoxLocation,20.f,FColor::Yellow,true);
	}
}

TArray<APathfindingGridNode*> APathfindingVolume::FindPath(APathfindingGridNode* StartNode,	APathfindingGridNode* EndNode)
{
	TArray<APathfindingGridNode*> OpenSet;
	TSet<APathfindingGridNode*> ClosedSet;
	TMap<APathfindingGridNode*, APathfindingGridNode*> CameFrom;

	StartNode->SetGCost(0.0f);
	StartNode->SetHCost(FVector::Dist(StartNode->GetActorLocation(), EndNode->GetActorLocation()));
	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0)
	{
		APathfindingGridNode* CurrentNode = OpenSet[0];
		for (APathfindingGridNode* Node : OpenSet)
		{
			if (Node->FCost() < CurrentNode->FCost())
			{
				CurrentNode = Node;
			}
		}

		if (CurrentNode == EndNode)
		{
			return GetPath(CurrentNode, CameFrom);
		}

		OpenSet.Remove(CurrentNode);
		ClosedSet.Add(CurrentNode);

		for (APathfindingGridNode* Neighbor : CurrentNode->GetNeighbors(GridNodes))
		{
			if (ClosedSet.Contains(Neighbor))
			{
				continue;
			}

			float TentativeGScore = CurrentNode->GetGCost() + FVector::Dist(CurrentNode->GetActorLocation(), Neighbor->GetActorLocation());

			if (!OpenSet.Contains(Neighbor))
			{
				OpenSet.Add(Neighbor);
			}
			else if (TentativeGScore >= Neighbor->GetGCost())
			{
				continue;
			}

			CameFrom.Add(Neighbor, CurrentNode);
			Neighbor->SetGCost(TentativeGScore);
			Neighbor->SetHCost(FVector::Dist(Neighbor->GetActorLocation(), EndNode->GetActorLocation()));
		}
	}

	return TArray<APathfindingGridNode*>();
}

TArray<APathfindingGridNode*> APathfindingVolume::GetPath(APathfindingGridNode*& CurrentNode, const TMap<APathfindingGridNode*,  APathfindingGridNode*>& CameFrom)
{
	TArray<APathfindingGridNode*> Path;
	while (CameFrom.Contains(CurrentNode))
	{
		Path.Add(CurrentNode);
		CurrentNode = CameFrom[CurrentNode];
	}
	
	Algo::Reverse(Path);

#if WITH_EDITOR
	for(APathfindingGridNode* Node: Path)
	{
		FColor BoxColor = FColor::Green;
		FVector BoxLocation = Node->GetBaseLocation() + FVector(0.0f, 0.0f, 5.f);
		DrawDebugBox(GetWorld(), BoxLocation, FVector(Node->GetNodeSize().X * 0.5f,Node->GetNodeSize().Y * 0.5f,1.f), BoxColor, false,3.f);
		DrawDebugPoint(GetWorld(),BoxLocation,10.f,BoxColor,false,3.f);
	}
#endif
	
	return Path;
}