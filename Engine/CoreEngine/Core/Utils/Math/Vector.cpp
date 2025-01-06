#include "Vector.h"

namespace JMath
{
	TVector TVector::operator-() const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVectorNegate(v1);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	TVector TVector::operator+(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorAdd(v1, v2);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	TVector TVector::operator-(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorSubtract(v1, v2);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	TVector TVector::operator*(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorMultiply(v1, v2);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	// TVector TVector::operator/(const TVector& V) const
	// {
	// 	XMVECTOR v1 = XMLoadFloat3(this);
	// 	XMVECTOR v2 = XMLoadFloat3(&V);
	// 	XMVECTOR X  = XMVectorDivide(v1, v2);
	// 	TVector  R;
	// 	XMStoreFloat3(&R, X);
	// 	return R;
	// }

	TVector TVector::operator|(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVector3Dot(v1, v2);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	TVector TVector::operator^(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVector3Cross(v1, v2);
		TVector  R;
		XMStoreFloat3(&R, X);
		return R;
	}

	TVector TVector::operator*(float Scale) const
	{
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR x  = XMVectorScale(v1, Scale);
		TVector        returnVec;
		XMStoreFloat3(&returnVec, x);
		return returnVec;
	}

	TVector TVector::operator*(TVector2 InVector2) const
	{
		return TVector(x * InVector2.x, y * InVector2.y, z);
	}

	bool TVector::operator==(const TVector& Other) const
	{
		return XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&Other));
	}

	bool TVector::operator!=(const TVector& Other) const
	{
		return XMVector3NotEqual(XMLoadFloat3(this), XMLoadFloat3(&Other));
	}

	TVector& TVector::operator+=(const TVector& V)
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorAdd(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	TVector& TVector::operator-=(const TVector& V)
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorSubtract(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	TVector& TVector::operator*=(float Scale)
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVectorScale(v1, Scale);
		XMStoreFloat3(this, X);
		return *this;
	}

	TVector& TVector::operator*=(const TVector& V)
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVectorMultiply(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}

	TVector& TVector::operator/=(const float Scale)
	{
		assert(Scale != 0.0f);

		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVectorScale(v1, 1.f / Scale);
		XMStoreFloat3(this, X);
		return *this;
	}

	float TVector::operator[](int32_t Index) const
	{
		if (Index == 0)
		{
			return x;
		}
		if (Index == 1)
		{
			return y;
		}
		if (Index == 2)
		{
			return z;
		}
		return x;
	}

	bool TVector::IsNearlyZero(float Tolerance) const
	{
		return abs(x) <= Tolerance && abs(y) <= Tolerance && abs(z) <= Tolerance;
	}

	bool TVector::IsNearlyEqual(const TVector& Other, float Tolerance) const
	{
		return operator-(Other).IsNearlyZero(Tolerance);
	}

	bool TVector::IsZero() const
	{
		return XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&ZeroVector));
	}

	bool TVector::InBounds(const TVector& Bounds) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&Bounds);
		return XMVector3InBounds(v1, v2);
	}

	float TVector::Length() const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVector3Length(v1);
		return XMVectorGetX(X);
	}

	float TVector::LengthSquared() const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVector3LengthSq(v1);
		return XMVectorGetX(X);
	}

	float TVector::Dot(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR X  = XMVector3Dot(v1, v2);
		return XMVectorGetX(X);
	}

	TVector TVector::Cross(const TVector& V) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR v2 = XMLoadFloat3(&V);
		XMVECTOR R  = XMVector3Cross(v1, v2);

		TVector result;
		XMStoreFloat3(&result, R);
		return result;
	}

	void TVector::Normalize()
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVector3Normalize(v1);
		XMStoreFloat3(this, X);
	}

	void TVector::Normalize(TVector& Result) const
	{
		XMVECTOR v1 = XMLoadFloat3(this);
		XMVECTOR X  = XMVector3Normalize(v1);
		XMStoreFloat3(&Result, X);
	}

	void TVector::ConvertToRadians()
	{
		x = XMConvertToRadians(x);
		y = XMConvertToRadians(y);
		z = XMConvertToRadians(z);
	}

	void TVector::ConvertToRadians(FVector& OutVector) const
	{
		OutVector.x = XMConvertToRadians(x);
		OutVector.y = XMConvertToRadians(y);
		OutVector.z = XMConvertToRadians(z);
	}

	void TVector::ConvertToDegrees()
	{
		x = XMConvertToDegrees(x);
		y = XMConvertToDegrees(y);
		z = XMConvertToDegrees(z);
	}

	TVector::TVector() noexcept
		: XMFLOAT3(0.f, 0.f, 0.f) {}


	TVector::TVector(float InF)
		: XMFLOAT3(InF, InF, InF) {}

	TVector::TVector(float InX, float InY, float InZ)
		: XMFLOAT3(InX, InY, InZ) {}


	TVector::TVector(const TVector2 V, float InZ) {}

	TVector::TVector(const DirectX::XMFLOAT3& InVector)
		: XMFLOAT3(InVector.x, InVector.y, InVector.z) {}

	TVector::TVector(const DirectX::XMVECTORF32& InVector)
		: XMFLOAT3(InVector.f[0], InVector.f[1], InVector.f[2]) {}

	TVector::TVector(DirectX::FXMVECTOR& InVector)
	{
		XMStoreFloat3(this, InVector);
	}

	TVector::TVector(const float* pArray)
		: XMFLOAT3(pArray) {}

	TVector& TVector::operator=(const DirectX::XMVECTORF32& InVector)
	{
		x = InVector.f[0];
		y = InVector.f[1];
		z = InVector.f[2];

		return *this;
	}

	TVector& TVector::operator=(DirectX::FXMVECTOR& InVector)
	{
		x = XMVectorGetX(InVector);
		y = XMVectorGetY(InVector);
		z = XMVectorGetZ(InVector);

		return *this;
	}

	float TVector::DistSquared(const TVector& V1, const TVector& V2)
	{
		return (V1 - V2).LengthSquared();
	}
}
