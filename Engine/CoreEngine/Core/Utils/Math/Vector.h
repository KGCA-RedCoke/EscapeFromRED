#pragma once
#include "Vector2.h"

namespace JMath
{

	struct TVector : public XMFLOAT3
	{
	public:
		static const TVector ZeroVector;
		static const TVector OneVector;

		static const TVector ForwardVector;
		static const TVector BackwardVector;
		static const TVector RightVector;
		static const TVector LeftVector;
		static const TVector UpVector;
		static const TVector DownVector;

		static const TVector XAxisVector;
		static const TVector YAxisVector;
		static const TVector ZAxisVector;

		static inline TVector Zero() { return ZeroVector; }
		static inline TVector One() { return OneVector; }
		static inline TVector UnitX() { return XAxisVector; }
		static inline TVector UnitY() { return YAxisVector; }
		static inline TVector UnitZ() { return ZAxisVector; }

		TVector operator-() const;

		TVector operator+(const TVector& V) const;
		TVector operator-(const TVector& V) const;
		TVector operator*(const TVector& V) const;
		// TVector operator/(const TVector& V) const;
		TVector operator|(const TVector& V) const;
		TVector operator^(const TVector& V) const;

		TVector operator*(float Scale) const;
		TVector operator*(TVector2 InVector2) const;

		// ------------------------ 비교 연산자 ----------------------------
		bool operator==(const TVector&) const;
		bool operator!=(const TVector&) const;

		// ------------------------ 연산자 ----------------------------
		TVector& operator+=(const TVector&);
		TVector& operator-=(const TVector&);
		TVector& operator*=(float Scale);
		TVector& operator*=(const TVector&);
		TVector& operator/=(const float Scale);

		float operator[](int32_t Index) const;

		// ------------------------ 단항 연산자 ----------------------------

		bool IsNearlyZero(float Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsNearlyEqual(const TVector& Other, float Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsZero() const;

		bool InBounds(const TVector& Bounds) const;

		float Length() const;
		float LengthSquared() const;

		float   Dot(const TVector& V) const;
		TVector Cross(const TVector& V) const;

		void Normalize();
		void Normalize(TVector& Result) const;

		void ConvertToRadians();
		void ConvertToRadians(FVector& OutVector) const;
		void ConvertToDegrees();

	public:
		// ------------------------ 생성자 ----------------------------
		TVector() noexcept;
		TVector(float InF);
		TVector(float InX, float InY, float InZ);

		explicit TVector(const TVector2 V, float InZ);
		explicit TVector(const DirectX::XMFLOAT3& InVector);
		explicit TVector(const DirectX::XMVECTORF32& InVector);
		explicit TVector(const DirectX::FXMVECTOR& InVector);
		TVector(const float* pArray);


		// ------------------------ 복사,이동 생성자 ----------------------------
		TVector(const TVector&) = default;
		TVector(TVector&&)      = default;

		// ------------------------ 대입 연산자 ----------------------------
		TVector& operator=(const DirectX::XMVECTORF32& InVector);
		TVector& operator=(const DirectX::FXMVECTOR& InVector);
		TVector& operator=(const TVector&) = default;
		TVector& operator=(TVector&&)      = default;

		operator DirectX::XMVECTOR() const { return XMLoadFloat3(this); }

		static float DistSquared(const TVector& V1, const TVector& V2);
	};

}
