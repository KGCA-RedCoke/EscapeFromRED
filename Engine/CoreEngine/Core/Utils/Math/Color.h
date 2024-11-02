#pragma once
#include "MathUtility.h"

struct FColor;

// float 0.f - 1.f
struct FLinearColor
{
	union
	{
		struct
		{
			float R,
				  G,
				  B,
				  A;
		};

		float RGBA[4];
	};

	static FLinearColor Alpha;
	static FLinearColor White;
	static FLinearColor White_HalfAlpha;
	static FLinearColor Red;
	static FLinearColor Green;
	static FLinearColor Blue;
	static FLinearColor TrueBlack;
	static FLinearColor Gray;
	static FLinearColor Lynch;
	static FLinearColor WhiteSmoke;
	static FLinearColor Gallary;
	static FLinearColor Iron;
	static FLinearColor Edward;
	static FLinearColor Cascade;
	static FLinearColor Silver;
	static FLinearColor Orange;
	static FLinearColor HippiePink;
	static FLinearColor GreenPea;
	static FLinearColor BlackPearl;
	static FLinearColor Shark;
	static FLinearColor EbonyClay;
	static FLinearColor Black;
	static FLinearColor Yellow;
	static FLinearColor YellowGreen;
	static FLinearColor Studio;
	static FLinearColor Cyan;
	static FLinearColor Magenta;

	static FLinearColor Blender_Grid_Gray;

	/** sRGB -> Linear ��� ���̺� */
	static float s_RGBToLinearTable[256];

	inline explicit FLinearColor()
		: R(0),
		  G(0),
		  B(0),
		  A(0) {}

	constexpr inline FLinearColor(float InR, float InG, float InB, float InA = 1.f)
		: R(InR),
		  G(InG),
		  B(InB),
		  A(InA) {}

	constexpr inline FLinearColor(const FColor& Color);


	inline static FLinearColor FromSRGBColor(const FColor& Color)
	{
		return FLinearColor(Color);
	}

	inline FLinearColor operator+(const FLinearColor& Other) const
	{
		return FLinearColor(
							this->R + Other.R,
							this->G + Other.G,
							this->B + Other.B,
							this->A + Other.A
						   );
	}

	inline FLinearColor& operator+=(const FLinearColor& Other)
	{
		R += Other.R;
		G += Other.G;
		B += Other.B;
		A += Other.A;
		return *this;
	}

	inline FLinearColor operator-(const FLinearColor& Other) const
	{
		return FLinearColor(
							this->R - Other.R,
							this->G - Other.G,
							this->B - Other.B,
							this->A - Other.A
						   );
	}

	inline FLinearColor& operator-=(const FLinearColor& Other)
	{
		R -= Other.R;
		G -= Other.G;
		B -= Other.B;
		A -= Other.A;
		return *this;
	}

	inline FLinearColor operator*(float Scalar) const
	{
		return FLinearColor(
							this->R * Scalar,
							this->G * Scalar,
							this->B * Scalar,
							this->A * Scalar
						   );
	}

	inline FLinearColor& operator*=(float Scalar)
	{
		R += Scalar;
		G += Scalar;
		B += Scalar;
		A += Scalar;
		return *this;
	}

	inline FLinearColor operator/(const FLinearColor& Other) const
	{
		return FLinearColor(
							this->R / Other.R,
							this->G / Other.G,
							this->B / Other.B,
							this->A / Other.A
						   );
	}

	inline FLinearColor& operator/=(const FLinearColor& Other)
	{
		R /= Other.R;
		G /= Other.G;
		B /= Other.B;
		A /= Other.A;
		return *this;
	}

	inline FLinearColor operator/(float Scalar) const
	{
		const float invScalar = 1.0f / Scalar;
		return FLinearColor(
							this->R * invScalar,
							this->G * invScalar,
							this->B * invScalar,
							this->A * invScalar
						   );
	}

	inline FLinearColor& operator/=(float Scalar)
	{
		const float invScalar = 1.0f / Scalar;
		R *= invScalar;
		G *= invScalar;
		B *= invScalar;
		A *= invScalar;
		return *this;
	}

	inline FLinearColor GetClamped(float InMin = 0.f, float InMax = 1.f) const
	{
		FLinearColor returnValue;

		returnValue.R = FMath::Clamp(R, InMin, InMax);
		returnValue.G = FMath::Clamp(G, InMin, InMax);
		returnValue.B = FMath::Clamp(B, InMin, InMax);
		returnValue.A = FMath::Clamp(A, InMin, InMax);

		return returnValue;
	}

	[[deprecated("Equals �̿�!")]] inline bool operator==(const FLinearColor& Other) const
	{
		return this->R == Other.R && this->G == Other.G && this->B == Other.B && this->A == Other.A;
	}

	[[deprecated("Equals �̿�!")]] inline bool operator!=(const FLinearColor& Other) const
	{
		return this->R != Other.R && this->G != Other.G && this->B != Other.B && this->A != Other.A;
	}

	inline bool Equals(const FLinearColor& Other, float Tolerance = M_KINDA_SMALL_NUMBER) const
	{
		return FMath::Abs(this->R - Other.R) < Tolerance && FMath::Abs(this->G - Other.G) < Tolerance &&
				FMath::Abs(this->B - Other.B) < Tolerance && FMath::Abs(this->A - Other.A) < Tolerance;
	}
};

// uint 0- 255
struct FColor
{

public:
	union
	{
		struct
		{
			uint8_t R, G, B, A;
		};

		uint32_t RGBA;
	};

	inline FColor()
	{
		R = G = B = 0;
	}

	// Double Word(32 ��Ʈ) <-> ������, �������� ���鿡�� DWColor�� ����ѵ�?
	uint32_t&       DWColor() { return RGBA; }
	const uint32_t& DWColor() const { return RGBA; }

	constexpr inline FColor(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA = 255)
		: R(InR),
		  G(InG),
		  B(InB),
		  A(InA) {}

	inline bool operator==(const FColor& Other) const
	{
		return DWColor() == Other.DWColor();
	}

	inline bool operator!=(const FColor& Other) const
	{
		return DWColor() != Other.DWColor();
	}

	inline void operator+=(const FColor& Other)
	{
		//  0 - 255 Clamp
		R = static_cast<uint8_t>(FMath::Min(static_cast<int32_t>(R) + static_cast<int32_t>(Other.R), 255));
		G = static_cast<uint8_t>(FMath::Min(static_cast<int32_t>(G) + static_cast<int32_t>(Other.G), 255));
		B = static_cast<uint8_t>(FMath::Min(static_cast<int32_t>(B) + static_cast<int32_t>(Other.B), 255));
		A = static_cast<uint8_t>(FMath::Min(static_cast<int32_t>(A) + static_cast<int32_t>(Other.A), 255));
	}


};
