#pragma once
#include "Core/Utils/Math/Vector4.h"

enum class ETraceType : uint8_t
{
	Pawn,
	BlockingVolume,
	Ground,
	Projectile,
	Max
};

enum class ECollisionType
{
	None,
	Quad,
	Plane,
	Ray,
	Box,
	Sphere,
	Capsule
};

class ICollision;

struct FHitResult
{
	ICollision* SrcCollision;
	ICollision* DstCollision;

	FVector HitLocation;
	FVector HitNormal;
	float   Distance;
};


DECLARE_DYNAMIC_DELEGATE(FOnComponentBeginOverlap, const FHitResult&);

DECLARE_DYNAMIC_DELEGATE(FOnComponentOverlap, const FHitResult&);

DECLARE_DYNAMIC_DELEGATE(FOnComponentEndOverlap, const FHitResult&);

class ICollision
{
public:
	FOnComponentBeginOverlap OnComponentBeginOverlap;
	FOnComponentOverlap      OnComponentOverlap;
	FOnComponentEndOverlap   OnComponentEndOverlap;

public:
	virtual uint32_t       GetCollisionID() = 0;
	virtual ETraceType     GetTraceType() const = 0;
	virtual ECollisionType GetType() const = 0;
	virtual bool           Intersect(const ICollision& InOther) const = 0;
};

class JMeshObject;

struct FRay : public ICollision
{
	FVector Origin;		// 광선의 시작점
	FVector Direction;	// 광선의 방향

	uint32_t       GetCollisionID() override;
	ETraceType     GetTraceType() const override;
	ECollisionType GetType() const override;
	bool           Intersect(const ICollision& InOther) const override;
};

struct FQuad : public ICollision
{
	FVector Center;	// 사각형의 중심
	FVector Extent;	// 사각형의 반지름

	FQuad() = default;

	FQuad(FVector InCenter, FVector InExtent)
		: Center(InCenter),
		  Extent(InExtent)
	{}

	uint32_t       GetCollisionID() override;
	ETraceType     GetTraceType() const override;
	ECollisionType GetType() const override;
	bool           Intersect(const ICollision& InOther) const override;

	bool Contains(const FVector& InPoint) const;
	void DrawDebug() const;
};

struct FPlane : public ICollision
{
	FVector Normal;		// 평면의 법선 벡터
	float   Distance;	// 원점으로부터 평면까지의 거리

	float A, B, C, D;	// Ax + By + Cz + D = 0 (평면의 방정식)

	uint32_t       GetCollisionID() override;
	ETraceType     GetTraceType() const override;
	ECollisionType GetType() const override;
	bool           Intersect(const ICollision& InOther) const override;

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

	/*
	uint32_t       GetCollisionID() override;
	ETraceType     GetTraceType() const override;
	ECollisionType GetType() const override;
	bool           Intersect(const ICollision& InOther) const override;*/

	bool Contains(const FVector& InPoint) const;
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
	bool IntersectOBB(const FBoxShape& InBox) const;
	bool Contains(const FVector& InPoint) const;

	FBoxShape() = default;

	FBoxShape(const FVector& InCenter, const FVector& InExtent)
	{
		Box.Center = InCenter;
		Box.Extent = InExtent;

		Min = Box.Center - Box.Extent;
		Max = Box.Center + Box.Extent;
	}
};

struct FSphere
{
	FVector Center; // 구의 중심
	float   Radius;   // 구의 반지름

	FSphere() = default;

	FSphere(const FVector& InCenter, float InRadius)
		: Center(InCenter),
		  Radius(InRadius)
	{}

	// 구와 구의 충돌 검사
	bool Intersect(const FSphere& InSphere) const;

	// 구와 점의 충돌 검사
	bool ContainsPoint(const FVector& Point) const;
};

struct FCapsule
{
	FVector Center = FVector::ZeroVector;  // 캡슐의 중심
	float   Radius = 50.f;    // 캡슐의 반지름 (둘레)
	float   Height = 100.f;    // 캡슐의 높이 (끝점 사이의 거리, 반구 부분 제외)

	FCapsule() = default;

	FCapsule(const FVector& InCenter, float InRadius, float InHeight)
		: Center(InCenter),
		  Radius(InRadius),
		  Height(InHeight) {}

	// 두 캡슐의 충돌 여부 검사
	bool Intersect(const FCapsule& Other) const;

	// 캡슐과 점 간 거리 계산
	float DistanceToPoint(const FVector& Point) const;

	// 디버그용 캡슐 그리기
	void DrawDebug() const;
};

static bool RayIntersectAABB(const FRay& InRay, const FBoxShape& InBox, float& OutT);
static bool RayIntersectOBB(const FRay& InRay, const FBoxShape& InBox, FVector& OutT);
static bool RayIntersectOBB(const FVector& RayOrigin, const FVector& RayDir,   // Ray 정보
							const FVector& BoxCenter, const FVector  BoxAxis[3], const FVector& BoxExtent, // OBB 정보
							FVector&       OutHitPoint // 충돌 지점
);
