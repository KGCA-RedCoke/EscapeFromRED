// #include "JShape.h"
//
// void JShape::CreateOBBBox(const FVector& InCenter, const FVector& InExtent, const FVector InAxisX, const FVector InAxisY,
// 						  const FVector  InAxisZ)
// {
// 	mBoxShape.Extent = InExtent;
//
// 	mBoxShape.Center = InCenter;
//
// 	mBoxShape.LocalAxis[0] = mBoxShape.Extent.x * InAxisX;
// 	mBoxShape.LocalAxis[1] = mBoxShape.Extent.y * InAxisY;
// 	mBoxShape.LocalAxis[2] = mBoxShape.Extent.z * InAxisZ;
//
// 	mBoxShape.Position[0] = mBoxShape.Center - mBoxShape.LocalAxis[0] - mBoxShape.LocalAxis[1] - mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[1] = mBoxShape.Center - mBoxShape.LocalAxis[0] + mBoxShape.LocalAxis[1] - mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[2] = mBoxShape.Center + mBoxShape.LocalAxis[0] + mBoxShape.LocalAxis[1] - mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[3] = mBoxShape.Center + mBoxShape.LocalAxis[0] - mBoxShape.LocalAxis[1] - mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[4] = mBoxShape.Center - mBoxShape.LocalAxis[0] - mBoxShape.LocalAxis[1] + mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[5] = mBoxShape.Center - mBoxShape.LocalAxis[0] + mBoxShape.LocalAxis[1] + mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[6] = mBoxShape.Center + mBoxShape.LocalAxis[0] + mBoxShape.LocalAxis[1] + mBoxShape.LocalAxis[2];
// 	mBoxShape.Position[7] = mBoxShape.Center + mBoxShape.LocalAxis[0] - mBoxShape.LocalAxis[1] + mBoxShape.LocalAxis[2];
//
// 	mBoxShape.Max = mBoxShape.Position[0];
// 	mBoxShape.Min = mBoxShape.Position[0];
//
// 	for (int32_t i = 1; i < 8; ++i)
// 	{
// 		if (mBoxShape.Max.x < mBoxShape.Position[i].x)
// 		{
// 			mBoxShape.Max.x = mBoxShape.Position[i].x;
// 		}
// 		if (mBoxShape.Max.y < mBoxShape.Position[i].y)
// 		{
// 			mBoxShape.Max.y = mBoxShape.Position[i].y;
// 		}
// 		if (mBoxShape.Max.z < mBoxShape.Position[i].z)
// 		{
// 			mBoxShape.Max.z = mBoxShape.Position[i].z;
// 		}
//
// 		if (mBoxShape.Min.x > mBoxShape.Position[i].x)
// 		{
// 			mBoxShape.Min.x = mBoxShape.Position[i].x;
// 		}
// 		if (mBoxShape.Min.y > mBoxShape.Position[i].y)
// 		{
// 			mBoxShape.Min.y = mBoxShape.Position[i].y;
// 		}
// 		if (mBoxShape.Min.z > mBoxShape.Position[i].z)
// 		{
// 			mBoxShape.Min.z = mBoxShape.Position[i].z;
// 		}
// 	}
//
// 	mBoxShape.LocalAxis[0].Normalize();
// 	mBoxShape.LocalAxis[1].Normalize();
// 	mBoxShape.LocalAxis[2].Normalize();
// }
//
// void JShape::CreateAABBBox(const FVector& InMin, const FVector& InMax)
// {
// 	mBoxShape.Min = InMin;
// 	mBoxShape.Max = InMax;
//
// 	mBoxShape.Center = (InMax + InMin) * 0.5f;
//
// 	mBoxShape.Position[0] = InMin;
// 	mBoxShape.Position[1] = FVector(InMin.x, InMax.y, InMin.z);
// 	mBoxShape.Position[2] = FVector(InMax.x, InMax.y, InMin.z);
// 	mBoxShape.Position[3] = FVector(InMax.x, InMin.y, InMin.z);
// 	mBoxShape.Position[4] = FVector(InMin.x, InMin.y, InMax.z);
// 	mBoxShape.Position[5] = FVector(InMin.x, InMax.y, InMax.z);
// 	mBoxShape.Position[6] = InMax;
// 	mBoxShape.Position[7] = FVector(InMax.x, InMin.y, InMax.z);
//
// 	// AABB이기 때문에 LocalAxis는 단위 벡터로 초기화
// 	mBoxShape.LocalAxis[0] = FVector(1.0f, 0.0f, 0.0f);
// 	mBoxShape.LocalAxis[1] = FVector(0.0f, 1.0f, 0.0f);
// 	mBoxShape.LocalAxis[2] = FVector(0.0f, 0.0f, 1.0f);
// }
//
// bool JBoxShape::CreateVertexData()
// {
// 	return false;
// }
//
// bool JBoxShape::CreateIndexData()
// {
// 	return false;
// }
//
// bool JBoxShape::CreateResource()
// {
// 	return false;
// }
