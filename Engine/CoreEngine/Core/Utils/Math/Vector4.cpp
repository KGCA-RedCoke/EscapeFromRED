#include "common_pch.h"
#include "Vector4.h"

namespace JMath
{

	TVector4::TVector4(XMFLOAT4 V)
		: XMFLOAT4(V) {}

	TVector4::TVector4(const XMFLOAT4& V)
		: XMFLOAT4(V) {}

	TVector4::TVector4(const XMVECTORF32& F)
	{
		x = F.f[0];
		y = F.f[1];
		z = F.f[2];
		w = F.f[3];
	}

	bool TVector4::operator==(const TVector4& V) const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4Equal(v1, v2);
	}

	bool TVector4::operator!=(const TVector4& V) const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		return XMVector4NotEqual(v1, v2);
	}

	TVector4& TVector4::operator=(const XMVECTORF32& F)
	{
		x = F.f[0];
		y = F.f[1];
		z = F.f[2];
		w = F.f[3];
		return *this;
	}

	TVector4& TVector4::operator+=(const TVector4& V)
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X  = XMVectorAdd(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	TVector4& TVector4::operator-=(const TVector4& V)
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X  = XMVectorSubtract(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	TVector4& TVector4::operator*=(const TVector4& V)
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X  = XMVectorMultiply(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}

	TVector4& TVector4::operator*=(float S)
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVectorScale(v1, S);
		XMStoreFloat4(this, X);
		return *this;
	}

	TVector4& TVector4::operator/=(float S)
	{
		assert(S != 0.0f);
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVectorScale(v1, 1.f / S);
		XMStoreFloat4(this, X);
		return *this;
	}

	TVector4 TVector4::operator-() const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVectorNegate(v1);
		TVector4 R;
		XMStoreFloat4(&R, X);
		return R;
	}

	float TVector4::Length() const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVector4Length(v1);
		return XMVectorGetX(X);
	}

	float TVector4::LengthSquared() const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVector4LengthSq(v1);
		return XMVectorGetX(X);
	}

	float TVector4::Dot(const TVector4& V) const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&V);
		XMVECTOR X  = XMVector4Dot(v1, v2);
		return XMVectorGetX(X);
	}

	void TVector4::Cross(const TVector4& v1, const TVector4& v2, TVector4& result) const
	{
		XMVECTOR x1 = XMLoadFloat4(this);
		XMVECTOR x2 = XMLoadFloat4(&v1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR R  = XMVector4Cross(x1, x2, x3);
		XMStoreFloat4(&result, R);
	}

	TVector4 TVector4::Cross(const TVector4& v1, const TVector4& v2) const
	{
		XMVECTOR x1 = XMLoadFloat4(this);
		XMVECTOR x2 = XMLoadFloat4(&v1);
		XMVECTOR x3 = XMLoadFloat4(&v2);
		XMVECTOR R  = XMVector4Cross(x1, x2, x3);

		TVector4 result;
		XMStoreFloat4(&result, R);
		return result;
	}

	void TVector4::Normalize()
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVector4Normalize(v1);
		XMStoreFloat4(this, X);
	}

	void TVector4::Normalize(TVector4& result) const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR X  = XMVector4Normalize(v1);
		XMStoreFloat4(&result, X);
	}

	void TVector4::Clamp(const TVector4& vmin, const TVector4& vmax)
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&vmin);
		XMVECTOR v3 = XMLoadFloat4(&vmax);
		XMVECTOR X  = XMVectorClamp(v1, v2, v3);
		XMStoreFloat4(this, X);
	}

	void TVector4::Clamp(const TVector4& vmin, const TVector4& vmax, TVector4& result) const
	{
		XMVECTOR v1 = XMLoadFloat4(this);
		XMVECTOR v2 = XMLoadFloat4(&vmin);
		XMVECTOR v3 = XMLoadFloat4(&vmax);
		XMVECTOR X  = XMVectorClamp(v1, v2, v3);
		XMStoreFloat4(&result, X);
	}
}
