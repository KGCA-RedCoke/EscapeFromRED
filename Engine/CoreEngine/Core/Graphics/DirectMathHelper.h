#pragma once
#include "graphics_common_include.h"
#include "Core/Utils/Math/Vector2.h"

inline FMatrix TranslationMatrix(float InX, float InY, float InZ = 0.f)
{
	return XMMatrixTranslation(InX, InY, InZ);
}

inline FMatrix RotationMatrix(float InDegree)
{
	float radians = XMConvertToRadians(InDegree);
	return XMMatrixRotationZ(radians);
}

inline FMatrix ScaleMatrix(float InX, float InY)
{
	return XMMatrixScaling(InX, InY, 1.0f);
}

/** 4x4 행렬 -> 위치벡터 반환 */
inline FVector Mat2LocVector(const FMatrix& InMatrix)
{
	XMFLOAT4X4 locationVec;
	XMStoreFloat4x4(&locationVec, InMatrix);
	return {locationVec._41, locationVec._42, locationVec._43};
}

/** 4x4 행렬 -> 회전 각 반환 */
inline float Mat2RotDegree(const FMatrix& InMatrix)
{
	XMFLOAT4X4 matrixValues;
	XMStoreFloat4x4(&matrixValues, InMatrix);

	return 0;
}

/** 4x4 행렬 -> 크기벡터 반환 */
inline FVector Mat2ScaleVector(const FMatrix& InMatrix)
{
	XMFLOAT4X4 matrixValues;
	XMStoreFloat4x4(&matrixValues, InMatrix);

	const float scaleX = sqrtf(matrixValues._11 * matrixValues._11 + matrixValues._12 * matrixValues._12 + matrixValues._13 * matrixValues._13);
	const float scaleY = sqrtf(matrixValues._21 * matrixValues._21 + matrixValues._22 * matrixValues._22 + matrixValues._23 * matrixValues._23);
	const float scaleZ = sqrtf(matrixValues._31 * matrixValues._31 + matrixValues._32 * matrixValues._32 + matrixValues._33 * matrixValues._33);

	return FVector{scaleX, scaleY, scaleZ};
}
