#pragma once

#include "GridNodeData.generated.h"

USTRUCT()
struct FGridNodeIndex
{
	GENERATED_BODY()
	
	int32 X = 0;
	int32 Y = 0;

	FGridNodeIndex() = default;
	FGridNodeIndex(int32 InX, int32 InY) : X(InX), Y(InY) {}

	bool operator==(const FGridNodeIndex& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FGridNodeIndex& Other) const
	{
		return X != Other.X || Y != Other.Y;
	}

	friend uint32 GetTypeHash(const FGridNodeIndex& Other)
	{

		return HashCombine(GetTypeHash(Other.X), GetTypeHash(Other.Y));
	}
};