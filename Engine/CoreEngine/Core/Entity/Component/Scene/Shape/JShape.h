#pragma once
#include "Core/Utils/Math/Vector.h"


class JMeshObject;

struct FPlane
{
	FVector Normal;		// 평면의 법선 벡터
	float   Distance;	// 원점으로부터 평면까지의 거리

	float A, B, C, D;	// Ax + By + Cz + D = 0 (평면의 방정식)

	void CreatePlane(FVector InNormal, FVector InPoint);
	void CreatePlane(FVector InPoint0, FVector InPoint1, FVector InPoint2);
	void Normalize();
};

struct FBox
{
	FVector Center = FVector::ZeroVector;			// 박스 중심좌표
	FVector Extent = FVector(50.f, 50.f, 50.f);			// 박스 반지름

	FVector LocalAxis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};	// 박스 로컬 축
	FVector Position[8];	// 박스의 8개의 꼭지점

	FBox() = default;

	FBox(const FVector& InCenter, const FVector& InExtent)
		: Center(InCenter),
		  Extent(InExtent)
	{}

	bool Intersect(const FBox& InBox) const;
};

struct FSphere
{
	FVector Center;	// 구의 중심
	float   Radius;	// 구의 반지름
};

struct FBoxShape
{
	// ------ Common ------
	FBox Box;

	// ------- OBB --------
	FVector Min = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector Max = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	void DrawDebug() const;

	bool Intersect(const FBoxShape& InBox) const;

	FBoxShape() = default;

	FBoxShape(const FVector& InCenter, const FVector& InExtent)
	{
		Box.Center = InCenter;
		Box.Extent = InExtent;

		Min = Box.Center - Box.Extent;
		Max = Box.Center + Box.Extent;
	}
};


// class JShape : public JSceneComponent
// {
// public:
// 	void CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
// 					  const FVector  InAxisZ);
// 	void CreateAABBBox(const FVector& InMin, const FVector& InMax);
//
// 	virtual void GenCollisionData() = 0;
// };
//
// class JBoxComponent : public JShape
// {
// public:
// 	void Initialize() override;
// 	void Tick(float DeltaTime) override;
//
// public:
// 	void SetExtent(const FVector& InExtent) { mBoxShape.Box.Extent = InExtent; }
// 	void SetCenter(const FVector& InCenter) { mBoxShape.Box.Center = InCenter; }
//
// 	[[nodiscard]] const FBoxShape& GetBoxShape() const { return mBoxShape; }
//
// private:
// 	FBoxShape mBoxShape;
// };
