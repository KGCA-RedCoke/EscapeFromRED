#include "JShape.h"

#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"

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

void FBoxShape::DrawDebug() const
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
	G_DebugBatch.DrawCube_Implement(mat, Colors::Green);
}
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
