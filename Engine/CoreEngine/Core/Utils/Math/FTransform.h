#pragma once
#include "Vector.h"
#include "Core/Graphics/graphics_common_include.h"

struct FTransform
{
	FVector Location;
	FVector Rotation;
	FVector Scale;
private:
	Matrix mMatrix;

public:
	FTransform();

	FTransform(FVector InLocation, FVector InRotation, FVector InScale)
		: Location(InLocation), Rotation(InRotation), Scale(InScale)
	{
		SetTransform(InLocation, InRotation, InScale);
	}

	FTransform(const FTransform& Other);

	// ------------------------ Transform --------------------------------
	void SetScale(const FVector& InScale);
	void SetRotation(const FVector& InRotation);
	void SetLocation(const FVector& InLocation);
	void AddLocation(const FVector& InAddLocation);
	void SetTransform(const Matrix& InMatrix);
	void SetTransform(const FVector& InLocation, const FVector& InAngle, const FVector& InScale);

	const FVector& GetLocation() const;
	const FVector& GetScale() const;
	float_t        GetRotation() const;
	// ------------------------ Transform --------------------------------

};

inline void FTransform::SetScale(const FVector& InScale)
{
	// 기존 행렬의 이동, 회전 성분을 저장
	XMVECTOR scale, rotation, translation;
	if (!XMMatrixDecompose(&scale, &rotation, &translation, mMatrix))
	{
		return;
	}

	// 새로운 스케일 행렬을 생성
	const XMMATRIX newScaleMatrix = XMMatrixScaling(InScale.X, InScale.Y, InScale.Z);

	// 새로운 스케일 행렬과 기존의 회전 행렬을 결합
	const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	mMatrix                       = newScaleMatrix * rotationMatrix;

	// 기존의 위치 성분을 다시 설정
	mMatrix.r[3] = translation;

	Scale = InScale;
}

inline void FTransform::SetRotation(const FVector& InRotation)
{
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(InRotation.X, InRotation.Y, InRotation.Z);
	mMatrix                       = XMMatrixMultiply(rotationMatrix, mMatrix);
	Rotation                      = InRotation;
}

inline void FTransform::SetLocation(const FVector& InLocation)
{
	mMatrix.r[3] = XMVectorSet(InLocation.X, InLocation.Y, InLocation.Z, 1.f);
}

inline void FTransform::AddLocation(const FVector& InAddLocation)
{
	mMatrix.r[3] = XMVectorSet(GetLocation().X + InAddLocation.X,
							   GetLocation().Y + InAddLocation.Y,
							   GetLocation().Z + InAddLocation.Z,
							   1.f);
}

inline void FTransform::SetTransform(const Matrix& InMatrix)
{
	mMatrix = InMatrix;
}

inline void FTransform::SetTransform(const FVector& InLocation, const FVector& InAngle, const FVector& InScale)
{
	
}

inline const FVector& FTransform::GetLocation() const
{}

inline const FVector& FTransform::GetScale() const {}
inline float_t        FTransform::GetRotation() const {}

