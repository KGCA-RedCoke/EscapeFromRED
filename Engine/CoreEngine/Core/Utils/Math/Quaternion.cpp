#include "Quaternion.h"

#include "TMatrix.h"

namespace JMath
{
	TQuaternion::TQuaternion() noexcept
		: XMFLOAT4(0, 0, 0, 1.f) {}

	TQuaternion::TQuaternion(float InX, float InY, float InZ, float InW)
		: XMFLOAT4(InX, InY, InZ, InW) {}

	TQuaternion::TQuaternion(const TVector& V, float Scalar)
		: XMFLOAT4(V.x, V.y, V.z, Scalar) {}

	TQuaternion::TQuaternion(const TVector4& V)
		: XMFLOAT4(V.x, V.y, V.z, V.w) {}

	TQuaternion::TQuaternion(const float* pArray)
		: XMFLOAT4(pArray) {}

	TQuaternion::TQuaternion(XMFLOAT4 V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = V.w;
	}

	TQuaternion::TQuaternion(const XMFLOAT4& InQuaternion)
	{
		x = InQuaternion.x;
		y = InQuaternion.y;
		z = InQuaternion.z;
		w = InQuaternion.w;
	}

	TQuaternion::TQuaternion(const DirectX::XMVECTORF32& F)
	{
		x = F.f[0];
		y = F.f[1];
		z = F.f[2];
		w = F.f[3];
	}

	bool TQuaternion::operator==(const TQuaternion& q) const
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		return XMQuaternionEqual(q1, q2);
	}

	bool TQuaternion::operator!=(const TQuaternion& q) const
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		return XMQuaternionNotEqual(q1, q2);
	}

	TQuaternion& TQuaternion::operator=(const DirectX::XMVECTORF32& F)
	{
		x = F.f[0];
		y = F.f[1];
		z = F.f[2];
		w = F.f[3];
		return *this;
	}

	TQuaternion& TQuaternion::operator+=(const TQuaternion& q)
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMVectorAdd(q1, q2));
		return *this;
	}

	TQuaternion& TQuaternion::operator-=(const TQuaternion& q)
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMVectorSubtract(q1, q2));
		return *this;
	}

	TQuaternion& TQuaternion::operator*=(const TQuaternion& q)
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));
		return *this;
	}

	TQuaternion& TQuaternion::operator*=(float S)
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMVectorScale(q, S));
		return *this;
	}

	TQuaternion& TQuaternion::operator/=(const TQuaternion& q)
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		q2          = XMQuaternionInverse(q2);
		XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));
		return *this;
	}

	TQuaternion TQuaternion::operator-() const
	{
		XMVECTOR q = XMLoadFloat4(this);

		TQuaternion R;
		XMStoreFloat4(&R, XMVectorNegate(q));
		return R;
	}

	float TQuaternion::Length() const
	{
		XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLength(q));
	}

	float TQuaternion::LengthSquared() const
	{
		XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLengthSq(q));
	}

	void TQuaternion::Normalize()
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMQuaternionNormalize(q));
	}

	void TQuaternion::Normalize(TQuaternion& result) const
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionNormalize(q));
	}

	void TQuaternion::Conjugate()
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMQuaternionConjugate(q));
	}

	void TQuaternion::Conjugate(TQuaternion& result) const
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionConjugate(q));
	}

	void TQuaternion::Inverse(TQuaternion& result) const
	{
		XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionInverse(q));
	}

	float TQuaternion::Dot(const TQuaternion& Q) const
	{
		XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&Q);
		return XMVectorGetX(XMQuaternionDot(q1, q2));
	}

	TQuaternion TQuaternion::CreateFromAxisAngle(const TVector& axis, float angle)
	{
		XMVECTOR a = XMLoadFloat3(&axis);

		TQuaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationAxis(a, angle));
		return R;
	}

	TQuaternion TQuaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
	{
		TQuaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
		return R;
	}

	TQuaternion TQuaternion::CreateFromRotationMatrix(const TMatrix& M)
	{
		XMMATRIX M0 = XMLoadFloat4x4(&M);

		TQuaternion R;
		XMStoreFloat4(&R, XMQuaternionRotationMatrix(M0));
		return R;
	}

	void TQuaternion::Lerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion& result)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		XMStoreFloat4(&result, XMQuaternionNormalize(R));
	}

	TQuaternion TQuaternion::Lerp(const TQuaternion& q1, const TQuaternion& q2, float t)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		TQuaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(R));
		return result;
	}

	void TQuaternion::Slerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion& result)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));
	}

	TQuaternion TQuaternion::Slerp(const TQuaternion& q1, const TQuaternion& q2, float t)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		TQuaternion result;
		XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));
		return result;
	}

	void TQuaternion::Concatenate(const TQuaternion& q1, const TQuaternion& q2, TQuaternion& result)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));
	}

	TQuaternion TQuaternion::Concatenate(const TQuaternion& q1, const TQuaternion& q2)
	{
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		TQuaternion result;
		XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));
		return result;
	}

	TQuaternion operator+(const TQuaternion& Q1, const TQuaternion& Q2)
	{
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		TQuaternion R;
		XMStoreFloat4(&R, XMVectorAdd(q1, q2));
		return R;
	}

	TQuaternion operator-(const TQuaternion& Q1, const TQuaternion& Q2)
	{
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		TQuaternion R;
		XMStoreFloat4(&R, XMVectorSubtract(q1, q2));
		return R;
	}

	TQuaternion operator*(const TQuaternion& Q1, const TQuaternion& Q2)
	{
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);

		TQuaternion R;
		XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));
		return R;
	}

	TQuaternion operator*(const TQuaternion& Q, float S)
	{
		XMVECTOR q = XMLoadFloat4(&Q);

		TQuaternion R;
		XMStoreFloat4(&R, XMVectorScale(q, S));
		return R;
	}

	TQuaternion operator/(const TQuaternion& Q1, const TQuaternion& Q2)
	{
		XMVECTOR q1 = XMLoadFloat4(&Q1);
		XMVECTOR q2 = XMLoadFloat4(&Q2);
		q2 = XMQuaternionInverse(q2);

		TQuaternion R;
		XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));
		return R;
	}

	TQuaternion operator*(float S, const TQuaternion& Q)
	{
		XMVECTOR q1 = XMLoadFloat4(&Q);

		TQuaternion R;
		XMStoreFloat4(&R, XMVectorScale(q1, S));
		return R;
	}
}
