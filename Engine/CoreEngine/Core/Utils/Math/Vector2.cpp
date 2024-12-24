#include "Vector2.h"

namespace JMath
{

	constexpr TVector2::TVector2(float InX, float InY)
		: XMFLOAT2(InX, InY) {}

	constexpr TVector2::TVector2(float InF)
		: XMFLOAT2(InF, InF) {}


	TVector2::TVector2(const float* pArray)
		: XMFLOAT2(pArray) {}

	TVector2::TVector2(XMFLOAT2 V)
		: XMFLOAT2(V) {}

	TVector2::TVector2(const XMFLOAT2& V)
		: XMFLOAT2(V) {}

	TVector2::TVector2(POINT InPoint)
		: XMFLOAT2(InPoint.x, InPoint.y) {}

	TVector2& TVector2::operator=(const DirectX::XMVECTORF32& F)
	{
		x = F.f[0];
		y = F.f[1];
		return *this;
	}

	TVector2& TVector2::operator=(const ImVec2& V)
	{
		x = V.x;
		y = V.y;
		return *this;
	}


	bool TVector2::operator==(const TVector2& V) const
	{
		return x == V.x && y == V.y;
	}

	bool TVector2::operator==(const ImVec2& V) const
	{
		return x == V.x && y == V.y;
	}


	bool TVector2::operator!=(const TVector2& V) const
	{
		return x != V.x && y != V.y;
	}

	bool TVector2::operator!=(const ImVec2& V) const
	{
		return x != V.x && y != V.y;
	}


	bool TVector2::operator<(const TVector2& Other) const
	{
		return x < Other.x && y < Other.y;
	}


	bool TVector2::operator<=(const TVector2& Other) const
	{
		return x <= Other.x && y <= Other.y;
	}


	bool TVector2::operator>(const TVector2& Other) const
	{
		return x > Other.x && y > Other.y;
	}


	bool TVector2::operator>=(const TVector2& Other) const
	{
		return x >= Other.x && y >= Other.y;
	}


	TVector2 TVector2::operator+=(const TVector2& Other)
	{
		x += Other.x;
		y += Other.y;
		return *this;
	}


	TVector2 TVector2::operator-=(const TVector2& Other)
	{
		x -= Other.x;
		y -= Other.y;
		return *this;
	}


	TVector2 TVector2::operator*=(const TVector2& Other)
	{
		x *= Other.x;
		y *= Other.y;
		return *this;
	}


	TVector2 TVector2::operator/=(const TVector2& Other)
	{
		x /= Other.x;
		y /= Other.y;
		return *this;
	}


	TVector2 TVector2::operator*=(float Scale)
	{
		x += Scale;
		y += Scale;
		return *this;
	}


	TVector2 TVector2::operator/=(float V)
	{
		const float RV = 1.f / V;
		x *= RV;
		y *= RV;
		return *this;
	}


	float TVector2::Distance(const TVector2& A, const TVector2& B)
	{
		return sqrt(DistanceSquared(A, B));
	}


	float TVector2::DistanceSquared(const TVector2& A, const TVector2& B)
	{
		return pow((B.x - A.x), 2) + pow((B.y - A.y), 2);
	}


	float TVector2::DotProduct(const TVector2& A, const TVector2& B)
	{
		return A | B;
	}


	float TVector2::CrossProduct(const TVector2& A, const TVector2& B)
	{
		return A ^ B;
	}


	TVector2 TVector2::Min(const TVector2& A, const TVector2& B)
	{
		return TVector2(A.x < B.x ? A.x : B.x, A.y < B.y ? A.y : B.y);
	}


	TVector2 TVector2::Max(const TVector2& A, const TVector2& B)
	{
		return TVector2(A.x > B.x ? A.x : B.x, A.y > B.y ? A.y : B.y);
	}


	float TVector2::GetMax() const
	{
		return x > y ? x : y;
	}


	float TVector2::GetMax_Abs() const
	{
		return abs(x) > abs(y) ? x : y;
	}


	float TVector2::GetMin() const
	{
		return x < y ? x : y;
	}


	float TVector2::GetMin_Abs() const
	{
		return abs(x) < abs(y) ? x : y;
	}


	float TVector2::GetLength() const
	{
		return sqrt(x * x + y * y);
	}


	float TVector2::GetLengthSquared() const
	{
		return x * x + y * y;
	}


	bool TVector2::Normalize(float Tolerance)
	{
		const float squareSum = GetLengthSquared();

		if (squareSum > Tolerance)
		{
			const float Scale = 1.f / sqrt(squareSum);
			x *= Scale;
			y *= Scale;

			return true;
		}

		x = 0.f;
		y = 0.f;

		return false;
	}


	TVector2 TVector2::GetSafeNormal(float Tolerance) const
	{
		const float squareSum = GetLength();

		if (squareSum > Tolerance)
		{
			const float Scale = 1.f / squareSum;

			return TVector2(x * Scale, y * Scale);
		}

		return ZeroVector;
	}


	bool TVector2::IsNearlyZero(float Tolerance) const
	{
		return abs(x) <= Tolerance && abs(y) <= Tolerance;
	}


	bool TVector2::IsNearlyEqual(const TVector2& Other, float Tolerance) const
	{
		return operator-(Other).IsNearlyZero(Tolerance);
	}


	bool TVector2::IsZero() const
	{
		return x == 0.f && y == 0.f;
	}


	std::string TVector2::ToString() const
	{
		return std::format("x={0:3.3f} y={1:3.3f}\n", x, y);
	}
}
