#pragma once
#include "Vector.h"

namespace JMath
{
	struct TVector4 : public XMFLOAT4
	{
	public:
		static const TVector4 ZeroVector;
		static const TVector4 OneVector;

		static const TVector4 UpVector;
		static const TVector4 DownVector;
		static const TVector4 ForwardVector;
		static const TVector4 BackwardVector;
		static const TVector4 RightVector;
		static const TVector4 LeftVector;

		static const TVector4 XAxisVector;
		static const TVector4 YAxisVector;
		static const TVector4 ZAxisVector;
		static const TVector4 WAxisVector;

		static inline TVector4 Zero() { return ZeroVector; }
		static inline TVector4 One() { return OneVector; }
		static inline TVector4 UnitX() { return XAxisVector; }
		static inline TVector4 UnitY() { return YAxisVector; }
		static inline TVector4 UnitZ() { return ZAxisVector; }
		static inline TVector4 UnitW() { return WAxisVector; }

		TVector4() noexcept {}
		constexpr explicit TVector4(float x) {}
		TVector4(TVector X, float Y);

		constexpr TVector4(float _x, float _y, float _z, float _w)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}

		explicit TVector4(_In_reads_(4) const float* pArray) {}
		TVector4(XMFLOAT4 V);
		TVector4(const XMFLOAT4& V);
		explicit TVector4(const XMVECTORF32& F);

		TVector4(const TVector4&)            = default;
		TVector4& operator=(const TVector4&) = default;

		TVector4(TVector4&&)            = default;
		TVector4& operator=(TVector4&&) = default;

		operator XMVECTOR() const { return XMLoadFloat4(this); }

		bool operator ==(const TVector4& V) const;
		bool operator !=(const TVector4& V) const;

		// Assignment operators
		TVector4& operator=(const XMVECTORF32& F);

		TVector4& operator+=(const TVector4& V);
		TVector4& operator-=(const TVector4& V);
		TVector4& operator*=(const TVector4& V);
		TVector4& operator*=(float S);
		TVector4& operator/=(float S);

		// Unary operators
		TVector4 operator+() const { return *this; }
		TVector4 operator-() const;

		float    Length() const;
		float    LengthSquared() const;

		float    Dot(const TVector4& V) const;
		void     Cross(const TVector4& v1, const TVector4& v2, TVector4& result) const;
		TVector4 Cross(const TVector4& v1, const TVector4& v2) const;

		void Normalize();
		void Normalize(TVector4& result) const;

		void Clamp(const TVector4& vmin, const TVector4& vmax);
		void Clamp(const TVector4& vmin, const TVector4& vmax, TVector4& result) const;
	};
}
