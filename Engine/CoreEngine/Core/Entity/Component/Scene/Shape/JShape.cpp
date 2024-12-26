#include "JShape.h"

#include "Collision/MCollisionManager.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"


void FRay::DrawDebug(DirectX::FXMVECTOR DebugColor) const
{
	G_DebugBatch.DrawRay_Implement(Origin, Direction, Length, true, DebugColor);
}

bool FQuad::Contains(const FVector& InPoint) const
{
	return
	(InPoint.x >= Center.x - Extent.x && InPoint.x <= Center.x + Extent.x &&
		InPoint.z >= Center.z - Extent.z && InPoint.z <= Center.z + Extent.z);

}

void FQuad::DrawDebug() const
{
	FVector4 v0 = {Center.x - Extent.x, 0.0f, Center.z - Extent.z, 1.0f};
	FVector4 v1 = {Center.x + Extent.x, 0.0f, Center.z - Extent.z, 1.0f};
	FVector4 v2 = {Center.x + Extent.x, 0.0f, Center.z + Extent.z, 1.0f};
	FVector4 v3 = {Center.x - Extent.x, 0.0f, Center.z + Extent.z, 1.0f};

	G_DebugBatch.DrawQuad_Implement(
									v0,
									v1,
									v2,
									v3,
									Colors::Green);

}

void FPlane::CreatePlane(FVector InNormal, FVector InPoint)
{
	InNormal.Normalize();

	A = InNormal.x;
	B = InNormal.y;
	C = InNormal.z;
	D = -(InNormal.x * InPoint.x + InNormal.y * InPoint.y + InNormal.z * InPoint.z);

	Normal   = InNormal;
	Distance = D;
}

void FPlane::CreatePlane(FVector InPoint0, FVector InPoint1, FVector InPoint2)
{
	FVector edge1  = InPoint1 - InPoint0;
	FVector edge2  = InPoint2 - InPoint0;
	FVector normal = edge1.Cross(edge2);
	normal.Normalize();

	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = -(A * InPoint0.x + B * InPoint0.y + C * InPoint0.z);

	Normal   = normal;
	Distance = D;
}

void FPlane::Normalize()
{
	float fMagnitude = sqrtf(Normal.x * Normal.x + Normal.y * Normal.y + Normal.z * Normal.z);
	Normal.x /= fMagnitude;
	Normal.y /= fMagnitude;
	Normal.z /= fMagnitude;
	Distance /= fMagnitude;

	A = Normal.x;
	B = Normal.y;
	C = Normal.z;
	D = Distance;
}


bool FBox::Contains(const FVector& InPoint) const
{
	// 계산을 미리 한 번만 수행하여 반복되는 계산을 줄임
	const float minX = Center.x - Extent.x;
	const float maxX = Center.x + Extent.x;
	const float minY = Center.y - Extent.y;
	const float maxY = Center.y + Extent.y;
	const float minZ = Center.z - Extent.z;
	const float maxZ = Center.z + Extent.z;

	// 빠른 종료: 점이 박스의 범위를 벗어난 경우 false
	if (InPoint.x < minX || InPoint.x > maxX)
	{
		return false;
	}
	if (InPoint.y < minY || InPoint.y > maxY)
	{
		return false;
	}
	if (InPoint.z < minZ || InPoint.z > maxZ)
	{
		return false;
	}

	return true;
}

void FBoxShape::DrawDebug(FXMVECTOR DebugColor) const
{
	// 스케일 행렬
	XMMATRIX scaleMatrix = XMMatrixScaling(Box.Extent.x, Box.Extent.y, Box.Extent.z);

	XMMATRIX rotationMatrix = XMMATRIX(
									   Box.LocalAxis[0].x,
									   Box.LocalAxis[0].y,
									   Box.LocalAxis[0].z,
									   0.0f,  // X 축
									   Box.LocalAxis[1].x,
									   Box.LocalAxis[1].y,
									   Box.LocalAxis[1].z,
									   0.0f,  // Y 축
									   Box.LocalAxis[2].x,
									   Box.LocalAxis[2].y,
									   Box.LocalAxis[2].z,
									   0.0f,  // Z 축
									   0.0f,
									   0.0f,
									   0.0f,
									   1.0f   // W 축
									  );

	// 위치 행렬
	XMMATRIX translationMatrix = XMMatrixTranslation(Box.Center.x, Box.Center.y, Box.Center.z);

	// 월드 행렬 결합
	XMMATRIX mat = scaleMatrix * rotationMatrix * translationMatrix;
	G_DebugBatch.DrawCube_Implement(mat, DebugColor);
}

bool FBoxShape::Intersect(const FBoxShape& InBox) const
{
	const auto otherMin = InBox.Box.Center - InBox.Box.Extent;
	const auto otherMax = InBox.Box.Center + InBox.Box.Extent;

	const auto thisMin = Box.Center - Box.Extent;
	const auto thisMax = Box.Center + Box.Extent;

	return !(otherMin.x > thisMax.x || otherMax.x < thisMin.x ||
		otherMin.y > thisMax.y || otherMax.y < thisMin.y ||
		otherMin.z > thisMax.z || otherMax.z < thisMin.z);
}

bool FBoxShape::IntersectOBB(const FBoxShape& Other, FHitResult& OutHitResult) const
{
	return BoxIntersectOBB(*this, Other, OutHitResult);
}

bool FBoxShape::Contains(const FVector& InPoint) const
{
	return Box.Contains(InPoint);
}

bool FSphere::Intersect(const FSphere& InSphere) const
{
	float distanceSquared = FVector::DistSquared(Center, InSphere.Center);
	float radiusSum       = Radius + InSphere.Radius;
	return distanceSquared <= (radiusSum * radiusSum);
}

bool FSphere::ContainsPoint(const FVector& Point) const
{
	return FVector::DistSquared(Center, Point) <= (Radius * Radius);
}

bool FCapsule::Intersect(const FCapsule& Other) const
{
	// // 두 캡슐의 중심축 두 끝점 계산
	// FVector thisTop    = Center + FVector(0, 0, Height / 2.0f);
	// FVector thisBottom = Center - FVector(0, 0, Height / 2.0f);
	//
	// FVector otherTop    = Other.Center + FVector(0, 0, Other.Height / 2.0f);
	// FVector otherBottom = Other.Center - FVector(0, 0, Other.Height / 2.0f);
	//
	// // 두 선분 간 최소 거리 계산
	// float DistanceSquared = FVector::SegmentToSegmentDistance(thisBottom, thisTop, otherBottom, otherTop);
	//
	// // 두 캡슐의 반지름을 합친 값과 거리 비교
	// const float CombinedRadius = Radius + Other.Radius;
	// return DistanceSquared <= (CombinedRadius * CombinedRadius);
	return false;
}

float FCapsule::DistanceToPoint(const FVector& Point) const
{
	// // 캡슐의 중심축 두 끝점 계산
	// FVector top    = Center + FVector(0, 0, Height / 2.0f);
	// FVector bottom = Center - FVector(0, 0, Height / 2.0f);
	//
	// // 점에서 선분까지의 거리 계산
	// FVector closestPointOnSegment;
	// float   distanceSquared = FVector::PointToSegmentDistance(Point, bottom, top, closestPointOnSegment);
	//
	// // 캡슐의 반지름을 고려한 최종 거리
	// return FMath::Max(0.0f, sqrt(distanceSquared) - Radius);
	return 0;
}

void FCapsule::DrawDebug() const
{
	// // 캡슐의 중심축 두 끝점 계산
	// FVector top    = Center + FVector(0, 0, Height / 2.0f);
	// FVector bottom = Center - FVector(0, 0, Height / 2.0f);
	//
	// // 디버그용 원/선 그리기 (외부 라이브러리 필요)

	// G_DebugBatch.DrawCapsule_Implement()
	// DebugDrawSphere(top, Radius);
	// DebugDrawSphere(bottom, Radius);
	// DebugDrawCylinder(bottom, top, Radius);


}

bool RayIntersectAABB(const FRay& InRay, const FBoxShape& InBox, float& OutT)
{

	float tMin = FLT_MIN /* -std::numeric_limits<float>::infinity();*/;
	float tMax = FLT_MAX /*std::numeric_limits<float>::infinity()*/;

	for (int i = 0; i < 3; ++i)
	{
		// 레이의 방향이 0인 경우 (축과 평행할 때)
		if (std::fabs(InRay.Direction[i]) < M_KINDA_SMALL_NUMBER)
		{
			// 레이의 원점이 박스의 범위를 벗어나면 교차하지 않음
			if (InRay.Origin[i] < InBox.Min[i] || InRay.Origin[i] > InBox.Max[i])
			{
				return false; // 박스 밖에 있음
			}
		}
		else // 레이의 방향이 0이 아닌 경우 (축과 평행하지 않을 때)
		{
			// 레이와 박스의 tmin, tmax 계산
			float invD = 1.0f / InRay.Direction[i];
			float t1   = (InBox.Min[i] - InRay.Origin[i]) * invD;
			float t2   = (InBox.Max[i] - InRay.Origin[i]) * invD;

			if (t1 > t2)
				std::swap(t1, t2); // t1이 항상 작은 값

			tMin = max(tMin, t1);
			tMax = min(tMax, t2);

			if (tMin > tMax)
				return false; // 교차하지 않음
		}
	}

	OutT = tMin; // 교차점까지의 거리
	return true;
}

bool RayIntersectOBB(const FVector& RayOrigin, const FVector& RayDir, const FVector& BoxCenter, const FVector BoxAxis[3],
					 const FVector& BoxExtent, FHitResult&    OutHitResult)
{
	float   tMin = 0.f; // Ray 시작 지점
	float   tMax = FLT_MAX; // Ray 최대 거리
	FVector HitPoint;

	// Ray와 각 축의 교차점 검사
	for (int i = 0; i < 3; i++)
	{
		FVector Axis = BoxAxis[i];
		float   e    = Axis.Dot(BoxCenter - RayOrigin);
		float   f    = Axis.Dot(RayDir);

		if (fabs(f) > M_KINDA_SMALL_NUMBER)
		{
			float t1 = (e - BoxExtent[i]) / f;
			float t2 = (e + BoxExtent[i]) / f;

			// t1이 t2보다 크면 스왑
			if (t1 > t2)
				std::swap(t1, t2);

			// tMin과 tMax 갱신
			tMin = FMath::Max(tMin, t1);
			tMax = FMath::Min(tMax, t2);

			// Ray가 박스를 벗어난 경우 충돌 없음
			if (tMin > tMax)
				return false;
		}
		else if (-e - BoxExtent[i] > 0.f || -e + BoxExtent[i] < 0.f)
		{
			// Ray가 축과 평행하지만 박스 외부에 있는 경우
			return false;
		}
	}

	// 충돌이 확인되면 충돌 지점 계산
	if (tMin > 0)
	{
		// OutHitResult = RayOrigin + tMin * RayDir; // Ray의 tMin 지점
		return true;
	}

	return false;
}

bool RayIntersectOBB(const FRay& InRay, const FBoxShape& InBox, FHitResult& OutHitResult)
{
	return RayIntersectOBB(InRay.Origin,
						   InRay.Direction,
						   InBox.Box.Center,
						   InBox.Box.LocalAxis,
						   InBox.Box.Extent,
						   OutHitResult);
}

bool BoxIntersectOBB(const FBoxShape& InBox, const FBoxShape& Other, FHitResult& OutHitResult)
{
	FVector boxCenterDist = InBox.Box.Center - Other.Box.Center; // 두 OBB의 중심 거리 벡터

	// A와 B의 로컬 축
	const FVector* axisA = InBox.Box.LocalAxis;
	const FVector* axisB = Other.Box.LocalAxis;

	// A의 Extent와 B의 Extent
	const FVector extentA = InBox.Box.Extent;
	const FVector extentB = Other.Box.Extent;

	// 축 간의 교차 외적 저장
	FVector crossAxis[3][3];

	// 모든 축에 대해 SAT 검사
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			crossAxis[i][j] = axisA[i].Cross(axisB[j]);
		}
	}

	// Helper: 투영 함수
	auto project = [](const FVector& Axis, const FVector& Extent, const FVector LocalAxis[3]) -> float{
		return fabs(Extent.x * Axis.Dot(LocalAxis[0])) +
				fabs(Extent.y * Axis.Dot(LocalAxis[1])) +
				fabs(Extent.z * Axis.Dot(LocalAxis[2]));
	};

	float   minOverlap = FLT_MAX;
	FVector bestAxis   = FVector::ZeroVector;

	// SAT(Separating Axis Theorem) 검사 루프
	// 	로컬 축 검사:  
	// axis[i] 축에 대해 두 박스의 중심 거리(boxCenterDist)를 투영
	// extent[i]는 박스의 axis[i] 축에 대한 반지름
	// project(axisA[i], extentB, axisB)는 B 박스의 axisA[i] 축에 대한 투영 길이
	// 두 박스의 투영 거리가 두 반지름의 합보다 크면 충돌하지 않음.
	for (int i = 0; i < 3; ++i)
	{
		// A의 로컬 축 검사
		float distance = fabs(boxCenterDist.Dot(axisA[i]));
		float projA    = extentA[i];
		float projB    = project(axisA[i], extentB, axisB);

		float overlap = (projA + projB) - distance;
		if (overlap < 0.0f)
			return false;

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			bestAxis   = axisA[i];
		}

		// B의 로컬 축 검사
		distance = fabs(boxCenterDist.Dot(axisB[i]));
		projA    = project(axisB[i], extentA, axisA);
		projB    = extentB[i];
		overlap  = (projA + projB) - distance;
		if (overlap < 0.0f)
			return false;

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			bestAxis   = axisB[i];
		}
	}

	// A와 B의 축 간 교차 검사
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			const FVector& axis = crossAxis[i][j];
			if (axis.LengthSquared() < M_KINDA_SMALL_NUMBER)
				continue; // 축이 0에 가까우면 무시

			const float distance = fabs(boxCenterDist.Dot(axis));
			const float projA    = project(axis, extentA, axisA);
			const float projB    = project(axis, extentB, axisB);
			float       overlap  = (projA + projB) - distance;
			if (overlap < 0.0f)
				return false;

			if (overlap < minOverlap)
			{
				minOverlap = overlap;
				bestAxis   = axis;
			}
		}
	}
	bestAxis.Normalize();
	// 충돌 정보를 저장
	OutHitResult.HitNormal   = bestAxis;
	OutHitResult.Distance    = minOverlap;
	OutHitResult.HitLocation = InBox.Box.Center - OutHitResult.HitNormal * (extentA.Length() - minOverlap * 0.5f);

	return true;
}

// Quad::FNode* JCollisionComponent::GetDivisionNode() const
// {
// 	if (!mOwnerActor)
// 		return nullptr;
//
// 	
// }


//
// void JShape::CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
// 						  const FVector  InAxisZ)
// {
// 	// mBoxShape.Box.Extent = InExtent;
// 	//
// 	// mBoxShape.Box.Center = InCenter;
// 	//
// 	// mBoxShape.Box.LocalAxis[0] = mBoxShape.Box.Extent.x * InAxisX;
// 	// mBoxShape.Box.LocalAxis[1] = mBoxShape.Box.Extent.y * InAxisY;
// 	// mBoxShape.Box.LocalAxis[2] = mBoxShape.Box.Extent.z * InAxisZ;
// 	//
// 	// mBoxShape.Box.Position[0] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] - mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[1] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] - mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[2] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] - mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[3] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] - mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[4] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] + mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[5] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] + mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[6] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] + mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	// mBoxShape.Box.Position[7] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] + mBoxShape.
// 	// 		Box.LocalAxis[2];
// 	//
// 	// mBoxShape.Max = mBoxShape.Box.Position[0];
// 	// mBoxShape.Min = mBoxShape.Box.Position[0];
// 	//
// 	// for (int32_t i = 1; i < 8; ++i)
// 	// {
// 	// 	if (mBoxShape.Max.x < mBoxShape.Box.Position[i].x)
// 	// 	{
// 	// 		mBoxShape.Max.x = mBoxShape.Box.Position[i].x;
// 	// 	}
// 	// 	if (mBoxShape.Max.y < mBoxShape.Box.Position[i].y)
// 	// 	{
// 	// 		mBoxShape.Max.y = mBoxShape.Box.Position[i].y;
// 	// 	}
// 	// 	if (mBoxShape.Max.z < mBoxShape.Box.Position[i].z)
// 	// 	{
// 	// 		mBoxShape.Max.z = mBoxShape.Box.Position[i].z;
// 	// 	}
// 	//
// 	// 	if (mBoxShape.Min.x > mBoxShape.Box.Position[i].x)
// 	// 	{
// 	// 		mBoxShape.Min.x = mBoxShape.Box.Position[i].x;
// 	// 	}
// 	// 	if (mBoxShape.Min.y > mBoxShape.Box.Position[i].y)
// 	// 	{
// 	// 		mBoxShape.Min.y = mBoxShape.Box.Position[i].y;
// 	// 	}
// 	// 	if (mBoxShape.Min.z > mBoxShape.Box.Position[i].z)
// 	// 	{
// 	// 		mBoxShape.Min.z = mBoxShape.Box.Position[i].z;
// 	// 	}
// 	// }
// 	//
// 	// mBoxShape.Box.LocalAxis[0].Normalize();
// 	// mBoxShape.Box.LocalAxis[1].Normalize();
// 	// mBoxShape.Box.LocalAxis[2].Normalize();
// }
//
// void JShape::CreateAABBBox(const FVector& InMin, const FVector& InMax)
// {
// 	// mBoxShape.Min = InMin;
// 	// mBoxShape.Max = InMax;
// 	//
// 	// // mBoxShape.Box.Center = 0.5F * FVector(InMax + InMin);
// 	//
// 	// mBoxShape.Box.Position[0] = InMin;
// 	// mBoxShape.Box.Position[1] = FVector(InMin.x, InMax.y, InMin.z);
// 	// mBoxShape.Box.Position[2] = FVector(InMax.x, InMax.y, InMin.z);
// 	// mBoxShape.Box.Position[3] = FVector(InMax.x, InMin.y, InMin.z);
// 	// mBoxShape.Box.Position[4] = FVector(InMin.x, InMin.y, InMax.z);
// 	// mBoxShape.Box.Position[5] = FVector(InMin.x, InMax.y, InMax.z);
// 	// mBoxShape.Box.Position[6] = InMax;
// 	// mBoxShape.Box.Position[7] = FVector(InMax.x, InMin.y, InMax.z);
// 	//
// 	// // AABB이기 때문에 LocalAxis는 단위 벡터로 초기화
// 	// mBoxShape.Box.LocalAxis[0] = FVector(1.0f, 0.0f, 0.0f);
// 	// mBoxShape.Box.LocalAxis[1] = FVector(0.0f, 1.0f, 0.0f);
// 	// mBoxShape.Box.LocalAxis[2] = FVector(0.0f, 0.0f, 1.0f);
// }
//
// void JBoxComponent::Initialize()
// {
// 	// mBoxMesh = MMeshManager::Get().CreateOrClone(Path_Mesh_Cube);
// 	// assert(mBoxMesh);
// }
//
// void JBoxComponent::Tick(float DeltaTime)
// {
// 	JShape::Tick(DeltaTime);
//
// 	mBoxShape.Box.Center = mLocalLocation;
//
// 	mBoxShape.Box.LocalAxis[0] = XMVector3TransformNormal(FVector(1, 0, 0), XMLoadFloat4x4(&mLocalMat));
// 	mBoxShape.Box.LocalAxis[1] = XMVector3TransformNormal(FVector(0, 1, 0), XMLoadFloat4x4(&mLocalMat));
// 	mBoxShape.Box.LocalAxis[2] = XMVector3TransformNormal(FVector(0, 0, 1), XMLoadFloat4x4(&mLocalMat));
//
// 	for (int32_t i = 0; i < 3; ++i)
// 	{
// 		mBoxShape.Box.LocalAxis[i].Normalize();
// 	}
// }
