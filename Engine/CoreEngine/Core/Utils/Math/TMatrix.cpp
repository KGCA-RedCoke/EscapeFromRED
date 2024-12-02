#include "TMatrix.h"

#include "Vector4.h"

namespace JMath
{

	TMatrix::TMatrix() noexcept
		: XMFLOAT4X4(1.f,
					 0,
					 0,
					 0,
					 0,
					 1.f,
					 0,
					 0,
					 0,
					 0,
					 1.f,
					 0,
					 0,
					 0,
					 0,
					 1.f) {}

	TMatrix::TMatrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13,
					 float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
		: XMFLOAT4X4(m00,
					 m01,
					 m02,
					 m03,
					 m10,
					 m11,
					 m12,
					 m13,
					 m20,
					 m21,
					 m22,
					 m23,
					 m30,
					 m31,
					 m32,
					 m33) {}

	TMatrix::TMatrix(const TVector& r0, const TVector& r1, const TVector& r2)
		: XMFLOAT4X4(r0.x,
					 r0.y,
					 r0.z,
					 0,
					 r1.x,
					 r1.y,
					 r1.z,
					 0,
					 r2.x,
					 r2.y,
					 r2.z,
					 0,
					 0,
					 0,
					 0,
					 1.f) {}

	TMatrix::TMatrix(const TVector4 r0, const TVector4& r1, const TVector4& r2, const TVector4& r3)
		: XMFLOAT4X4(r0.x,
					 r0.y,
					 r0.z,
					 r0.w,
					 r1.x,
					 r1.y,
					 r1.z,
					 r1.w,
					 r2.x,
					 r2.y,
					 r2.z,
					 r2.w,
					 r3.x,
					 r3.y,
					 r3.z,
					 r3.w) {}

	TMatrix::TMatrix(const XMFLOAT4X4& M)
	{
		memcpy_s(this, sizeof(float) * 16, &M, sizeof(XMFLOAT4X4));
	}

	TMatrix::TMatrix(const XMFLOAT3X3& M)
	{
		_11 = M._11;
		_12 = M._12;
		_13 = M._13;
		_14 = 0.f;
		_21 = M._21;
		_22 = M._22;
		_23 = M._23;
		_24 = 0.f;
		_31 = M._31;
		_32 = M._32;
		_33 = M._33;
		_34 = 0.f;
		_41 = 0.f;
		_42 = 0.f;
		_43 = 0.f;
		_44 = 1.f;
	}

	TMatrix::TMatrix(const XMFLOAT4X3& M)
	{
		_11 = M._11;
		_12 = M._12;
		_13 = M._13;
		_14 = 0.f;
		_21 = M._21;
		_22 = M._22;
		_23 = M._23;
		_24 = 0.f;
		_31 = M._31;
		_32 = M._32;
		_33 = M._33;
		_34 = 0.f;
		_41 = M._41;
		_42 = M._42;
		_43 = M._43;
		_44 = 1.f;
	}

	TMatrix::TMatrix(const float* pArray)
		: XMFLOAT4X4(pArray) {}

	TMatrix::TMatrix(CXMMATRIX M)
	{
		XMStoreFloat4x4(this, M);
	}

	bool TMatrix::operator==(const TMatrix& M) const
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		return (XMVector4Equal(x1, y1)
			&& XMVector4Equal(x2, y2)
			&& XMVector4Equal(x3, y3)
			&& XMVector4Equal(x4, y4)) != 0;
	}

	bool TMatrix::operator!=(const TMatrix& M) const
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		return (XMVector4NotEqual(x1, y1)
			|| XMVector4NotEqual(x2, y2)
			|| XMVector4NotEqual(x3, y3)
			|| XMVector4NotEqual(x4, y4)) != 0;
	}

	TMatrix& TMatrix::operator=(const XMFLOAT3X3& M)
	{
		_11 = M._11;
		_12 = M._12;
		_13 = M._13;
		_14 = 0.f;
		_21 = M._21;
		_22 = M._22;
		_23 = M._23;
		_24 = 0.f;
		_31 = M._31;
		_32 = M._32;
		_33 = M._33;
		_34 = 0.f;
		_41 = 0.f;
		_42 = 0.f;
		_43 = 0.f;
		_44 = 1.f;
		return *this;
	}

	TMatrix& TMatrix::operator=(const XMFLOAT4X3& M)
	{
		_11 = M._11;
		_12 = M._12;
		_13 = M._13;
		_14 = 0.f;
		_21 = M._21;
		_22 = M._22;
		_23 = M._23;
		_24 = 0.f;
		_31 = M._31;
		_32 = M._32;
		_33 = M._33;
		_34 = 0.f;
		_41 = M._41;
		_42 = M._42;
		_43 = M._43;
		_44 = 1.f;
		return *this;
	}

	TMatrix& TMatrix::operator+=(const TMatrix& M)
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorAdd(x1, y1);
		x2 = XMVectorAdd(x2, y2);
		x3 = XMVectorAdd(x3, y3);
		x4 = XMVectorAdd(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	TMatrix& TMatrix::operator-=(const TMatrix& M)
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorSubtract(x1, y1);
		x2 = XMVectorSubtract(x2, y2);
		x3 = XMVectorSubtract(x3, y3);
		x4 = XMVectorSubtract(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	TMatrix& TMatrix::operator*=(const TMatrix& M)
	{
		XMMATRIX M1 = XMLoadFloat4x4(this);
		XMMATRIX M2 = XMLoadFloat4x4((XMFLOAT4X4*)&m);
		XMMATRIX X  = XMMatrixMultiply(M1, M2);
		XMStoreFloat4x4(this, X);
		return *this;
	}

	TMatrix& TMatrix::operator*=(float S)
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	TMatrix& TMatrix::operator/=(float S)
	{
		assert(S != 0.f);
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		float rs = 1.f / S;

		x1 = XMVectorScale(x1, rs);
		x2 = XMVectorScale(x2, rs);
		x3 = XMVectorScale(x3, rs);
		x4 = XMVectorScale(x4, rs);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	TMatrix& TMatrix::operator/=(const TMatrix& M)
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorDivide(x1, y1);
		x2 = XMVectorDivide(x2, y2);
		x3 = XMVectorDivide(x3, y3);
		x4 = XMVectorDivide(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	TMatrix TMatrix::operator-() const
	{
		XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		v1 = XMVectorNegate(v1);
		v2 = XMVectorNegate(v2);
		v3 = XMVectorNegate(v3);
		v4 = XMVectorNegate(v4);

		TMatrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), v1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), v2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), v3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), v4);
		return R;
	}


	TMatrix TMatrix::operator*(const TMatrix& M)
	{
		XMMATRIX m1 = XMLoadFloat4x4(this);
		XMMATRIX m2 = XMLoadFloat4x4((XMFLOAT4X4*)&M);
		XMMATRIX X  = XMMatrixMultiply(m1, m2);

		TMatrix R;
		XMStoreFloat4x4(&R, X);
		return R;
	}

	TMatrix TMatrix::operator*(TMatrix& M)
	{
		XMMATRIX m1 = XMLoadFloat4x4(this);
		XMMATRIX m2 = XMLoadFloat4x4((XMFLOAT4X4*)&M);
		XMMATRIX X  = XMMatrixMultiply(m1, m2);

		TMatrix R;
		XMStoreFloat4x4(&R, X);
		return R;
	}

	TMatrix TMatrix::operator*(float Scale)
	{
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		x1 = XMVectorScale(x1, Scale);
		x2 = XMVectorScale(x2, Scale);
		x3 = XMVectorScale(x3, Scale);
		x4 = XMVectorScale(x4, Scale);

		TMatrix R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}

	TVector TMatrix::Up() const
	{
		return FVector(_21, _22, _23);
	}

	void TMatrix::Up(const TVector& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}

	TVector TMatrix::Down() const
	{
		return TVector(-_21, -_22, -_23);
	}

	void TMatrix::Down(const TVector& v)
	{
		_21 = -v.x;
		_22 = -v.y;
		_23 = -v.z;
	}

	TVector TMatrix::Right() const
	{
		return TVector(_11, _12, _13);
	}

	void TMatrix::Right(const TVector& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	TVector TMatrix::Left() const
	{
		return TVector(-_11, -_12, -_13);
	}

	void TMatrix::Left(const TVector& v)
	{
		_11 = -v.x;
		_12 = -v.y;
		_13 = -v.z;
	}

	TVector TMatrix::Forward() const
	{
		return TVector(-_31, -_32, -_33);
	}

	void TMatrix::Forward(const TVector& v)
	{
		_31 = -v.x;
		_32 = -v.y;
		_33 = -v.z;
	}

	TVector TMatrix::Backward() const
	{
		return TVector(_31, _32, _33);
	}

	void TMatrix::Backward(const TVector& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	TVector TMatrix::Translation() const
	{
		return TVector(_41, _42, _43);
	}

	void TMatrix::Translation(const TVector& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	bool TMatrix::Decompose(TVector& scale, TQuaternion& rotation, TVector& translation)
	{
		XMVECTOR s, r, t;

		if (!XMMatrixDecompose(&s, &r, &t, *(XMMATRIX*)this))
			return false;

		XMStoreFloat3(&scale, s);
		XMStoreFloat4(&rotation, r);
		XMStoreFloat3(&translation, t);

		return true;
	}

	TMatrix TMatrix::Transpose() const
	{
		FXMMATRIX M = XMLoadFloat4x4(this);
		TMatrix   R;
		XMStoreFloat4x4(&R, XMMatrixTranspose(M));
		return R;
	}

	void TMatrix::Transpose(TMatrix& result) const
	{
		FXMMATRIX M = XMLoadFloat4x4(this);
		XMStoreFloat4x4(&result, XMMatrixTranspose(M));
	}

	TMatrix TMatrix::Invert() const
	{
		FXMMATRIX M = XMLoadFloat4x4(this);
		TMatrix   R;
		XMVECTOR  det;
		XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
		return R;
	}

	void TMatrix::Invert(TMatrix& result) const
	{
		FXMMATRIX M = XMLoadFloat4x4(this);
		XMVECTOR  det;
		XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
	}

	float TMatrix::Determinant() const
	{
		FXMMATRIX M = XMLoadFloat4x4(this);
		return XMVectorGetX(XMMatrixDeterminant(M));
	}
}
