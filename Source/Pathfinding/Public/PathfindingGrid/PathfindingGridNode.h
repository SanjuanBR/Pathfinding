// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNodeData.h"
#include "GameFramework/Actor.h"
#include "PathfindingGridNode.generated.h"


UCLASS()
class PATHFINDING_API APathfindingGridNode : public AActor
{
	GENERATED_BODY()
	
public:	
	APathfindingGridNode();

	bool IsWalkable() const { return bIsWalkable; }

	void InitializeNode(const FVector& NewNodeSize);
	void SetCoordinate(const FGridNodeIndex& NewGridNodeIndex) { GridNodeIndex = NewGridNodeIndex; }
	
	TArray<APathfindingGridNode*> GetNeighbors(const TMap<FGridNodeIndex, APathfindingGridNode*>& GridNodes) const;
	
	float FCost() const { return GCost + HCost; }
	
	void SetHCost(const float NewHCost) { HCost = NewHCost; }
	void SetGCost(const float NewGCost) { GCost = NewGCost; }
	
	float GetGCost() const { return GCost; }
	float GetHCost() const { return HCost; }

	FVector GetNodeSize() const { return NodeSize; }

	FVector GetBaseLocation() const { return GetActorLocation() - FVector(0.0f, 0.0f, NodeSize.Z * 0.5f); };

protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	bool bIsWalkable = false;

	UPROPERTY()
	float GCost;

	UPROPERTY()
	float HCost;

	UPROPERTY()
	FVector NodeSize;

	UPROPERTY()
	FGridNodeIndex GridNodeIndex;

	void SetInSubsystem();
};
