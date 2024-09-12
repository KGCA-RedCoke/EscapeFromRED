#pragma once
#include <DirectXMath.h>
#include "MathUtility.h"
#include "Debug/Assert.h"

namespace JMath
{
	using namespace DirectX;

	struct TVector2 : public XMFLOAT2
	{
	public:
		static const TVector2 ZeroVector;
		static const TVector2 UnitVector;
		static const TVector2 Unit45Deg;

		static inline TVector2 Zero() { return ZeroVector; }
		static inline TVector2 One() { return UnitVector; }
		static inline TVector2 UnitX() { return TVector2(1, 0); }
		static inline TVector2 UnitY() { return TVector2(0, 1); }

	public:
		TVector2() noexcept = default;
		constexpr          TVector2(float InX, float InY);
		constexpr explicit TVector2(float InF);
		explicit           TVector2(_In_reads_(2) const float* pArray);
		TVector2(XMFLOAT2 V);
		TVector2(const XMFLOAT2& V);
		TVector2(POINT InPoint);
		// explicit TVector2(const TVector<float>& V);
		TVector2& operator=(const DirectX::XMVECTORF32& F);
		operator DirectX::XMVECTOR() const { return XMLoadFloat2(this); }

		inline TVector2 operator+(const TVector2& V) const;
		inline TVector2 operator-(const TVector2& V) const;
		inline TVector2 operator*(const TVector2& V) const;
		inline float    operator|(const TVector2& V) const;
		inline float    operator^(const TVector2& V) const;
		TVector2        operator/(const TVector2& V) const;

		inline TVector2 operator+(float A) const;
		inline TVector2 operator-(float A) const;
		inline TVector2 operator*(float Scale) const;
		TVector2        operator/(float Scale) const;

		inline bool operator==(const TVector2& V) const;
		inline bool operator!=(const TVector2& V) const;

		inline bool operator<(const TVector2& Other) const;
		inline bool operator<=(const TVector2& Other) const;
		inline bool operator>(const TVector2& Other) const;
		inline bool operator>=(const TVector2& Other) const;

		inline TVector2 operator+=(const TVector2& Other);
		inline TVector2 operator-=(const TVector2& Other);
		inline TVector2 operator*=(const TVector2& Other);
		TVector2        operator/=(const TVector2& Other);

		inline TVector2 operator*=(float Scale);
		TVector2        operator/=(float V);

	public:
		inline static float Distance(const TVector2& A, const TVector2& B);
		inline static float DistanceSquared(const TVector2& A, const TVector2& B);

		inline static float DotProduct(const TVector2& A, const TVector2& B);
		inline static float CrossProduct(const TVector2& A, const TVector2& B);

		inline static TVector2 Min(const TVector2& A, const TVector2& B);
		inline static TVector2 Max(const TVector2& A, const TVector2& B);

		float GetMax() const;
		float GetMax_Abs() const;

		float GetMin() const;
		float GetMin_Abs() const;

		inline float GetLength() const;
		inline float GetLengthSquared() const;

		bool     Normalize(float Tolerance = M_SMALL_NUMBER);
		TVector2 GetSafeNormal(float Tolerance = M_SMALL_NUMBER) const;

		bool IsNearlyZero(float Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsNearlyEqual(const TVector2& Other, float Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsZero() const;

		inline std::string ToString() const;
	};


}
