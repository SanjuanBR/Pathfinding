// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNodeData.h"
#include "GameFramework/Volume.h"
#include "PathfindingVolume.generated.h"

class APathfindingGridNode;
class UBoxComponent;

UCLASS()
class PATHFINDING_API APathfindingVolume : public AActor
{
	GENERATED_BODY()

public:
	APathfindingVolume();
	
	void InitializeGrid();

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	APathfindingGridNode* GetNearestNode(const FVector& Location) const;

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	TArray<APathfindingGridNode*> FindPath(APathfindingGridNode* StartNode, APathfindingGridNode* EndNode);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APathfindingGridNode> GridNodeClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float NodeSize = 50.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Setup|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug = true;
#endif

	UPROPERTY()
	TMap<FGridNodeIndex, APathfindingGridNode*> GridNodes;

	void TestNeighbor(FGridNodeIndex NodeIndex, FColor Color = FColor::Green);
	void SetInSubsystem();

	TArray<APathfindingGridNode*> GetPath(APathfindingGridNode*& CurrentNode,const TMap<APathfindingGridNode*, APathfindingGridNode*>& CameFrom);
};
