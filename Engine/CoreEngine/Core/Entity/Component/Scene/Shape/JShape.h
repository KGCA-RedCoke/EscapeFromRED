#pragma once
#include "../JSceneComponent.h"

class JMeshObject;

struct FPlane
{
	FVector Normal;		// 평면의 법선 벡터
	float   Distance;	// 원점으로부터 평면까지의 거리
};

struct FBox
{
	FVector Center;			// 박스 중심좌표
	FVector Extent;			// 박스 반지름

	FVector LocalAxis[3];	// 박스 로컬 축
	FVector Position[8];	// 박스의 8개의 꼭지점
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
	FVector Min;
	FVector Max;
};


class JShape : public JSceneComponent
{
public:
	void CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
					  const FVector  InAxisZ);
	void CreateAABBBox(const FVector& InMin, const FVector& InMax);

	virtual void GenCollisionData() = 0;

protected:
	FBoxShape mBoxShape;
};

class JBoxComponent : public JShape
{
public:
	void Initialize() override;
	void GenCollisionData() override;

protected:
	JMeshObject* mBoxMesh;
};
