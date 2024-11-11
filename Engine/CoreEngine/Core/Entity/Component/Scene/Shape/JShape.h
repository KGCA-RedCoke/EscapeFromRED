#pragma once
#include "../JSceneComponent.h"

struct FBoxShape
{
	// ------ Common ------
	FVector Center;
	FVector Position[8];

	// ------- OBB --------
	FVector Min;
	FVector Max;

	// ------- AABB --------
	FVector LocalAxis[3];
	FVector Extent;
};


class JShape : public JSceneComponent
{
public:
	void CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
					  const FVector  InAxisZ);
	void CreateAABBBox(const FVector& InMin, const FVector& InMax);

protected:
	FBoxShape mBoxShape;
};

class JBoxShape : public JShape
{
public:
	bool CreateVertexData();
	bool CreateIndexData();
	bool CreateResource();
};
