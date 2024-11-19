#include "JShape.h"

#include "Core/Graphics/Mesh/MMeshManager.h"

void JShape::CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
						  const FVector  InAxisZ)
{
	mBoxShape.Box.Extent = InExtent;

	mBoxShape.Box.Center = InCenter;

	mBoxShape.Box.LocalAxis[0] = mBoxShape.Box.Extent.x * InAxisX;
	mBoxShape.Box.LocalAxis[1] = mBoxShape.Box.Extent.y * InAxisY;
	mBoxShape.Box.LocalAxis[2] = mBoxShape.Box.Extent.z * InAxisZ;

	// mBoxShape.Box.Position[0] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] - mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[1] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] - mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[2] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] - mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[3] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] - mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[4] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] + mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[5] = mBoxShape.Box.Center - mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] + mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[6] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] + mBoxShape.Box.LocalAxis[1] + mBoxShape.
	// 		Box.LocalAxis[2];
	// mBoxShape.Box.Position[7] = mBoxShape.Box.Center + mBoxShape.Box.LocalAxis[0] - mBoxShape.Box.LocalAxis[1] + mBoxShape.
	// 		Box.LocalAxis[2];

	mBoxShape.Max = mBoxShape.Box.Position[0];
	mBoxShape.Min = mBoxShape.Box.Position[0];

	for (int32_t i = 1; i < 8; ++i)
	{
		if (mBoxShape.Max.x < mBoxShape.Box.Position[i].x)
		{
			mBoxShape.Max.x = mBoxShape.Box.Position[i].x;
		}
		if (mBoxShape.Max.y < mBoxShape.Box.Position[i].y)
		{
			mBoxShape.Max.y = mBoxShape.Box.Position[i].y;
		}
		if (mBoxShape.Max.z < mBoxShape.Box.Position[i].z)
		{
			mBoxShape.Max.z = mBoxShape.Box.Position[i].z;
		}

		if (mBoxShape.Min.x > mBoxShape.Box.Position[i].x)
		{
			mBoxShape.Min.x = mBoxShape.Box.Position[i].x;
		}
		if (mBoxShape.Min.y > mBoxShape.Box.Position[i].y)
		{
			mBoxShape.Min.y = mBoxShape.Box.Position[i].y;
		}
		if (mBoxShape.Min.z > mBoxShape.Box.Position[i].z)
		{
			mBoxShape.Min.z = mBoxShape.Box.Position[i].z;
		}
	}

	mBoxShape.Box.LocalAxis[0].Normalize();
	mBoxShape.Box.LocalAxis[1].Normalize();
	mBoxShape.Box.LocalAxis[2].Normalize();
}

void JShape::CreateAABBBox(const FVector& InMin, const FVector& InMax)
{
	// mBoxShape.Min = InMin;
	// mBoxShape.Max = InMax;
	//
	// mBoxShape.Box.Center = (InMax + InMin) * 0.5f;
	//
	// mBoxShape.Box.Position[0] = InMin;
	// mBoxShape.Box.Position[1] = FVector(InMin.x, InMax.y, InMin.z);
	// mBoxShape.Box.Position[2] = FVector(InMax.x, InMax.y, InMin.z);
	// mBoxShape.Box.Position[3] = FVector(InMax.x, InMin.y, InMin.z);
	// mBoxShape.Box.Position[4] = FVector(InMin.x, InMin.y, InMax.z);
	// mBoxShape.Box.Position[5] = FVector(InMin.x, InMax.y, InMax.z);
	// mBoxShape.Box.Position[6] = InMax;
	// mBoxShape.Box.Position[7] = FVector(InMax.x, InMin.y, InMax.z);
	//
	// // AABB이기 때문에 LocalAxis는 단위 벡터로 초기화
	// mBoxShape.Box.LocalAxis[0] = FVector(1.0f, 0.0f, 0.0f);
	// mBoxShape.Box.LocalAxis[1] = FVector(0.0f, 1.0f, 0.0f);
	// mBoxShape.Box.LocalAxis[2] = FVector(0.0f, 0.0f, 1.0f);
}

void JBoxComponent::Initialize()
{
	mBoxMesh = MMeshManager::Get().CreateOrClone(Path_Mesh_Cube);
	assert(mBoxMesh);
}

void JBoxComponent::GenCollisionData()
{
	
}

