#pragma once
#include "Vector4.h"

namespace JMath
{
    struct TQuaternion : public DirectX::XMFLOAT4
    {
        static const TQuaternion Identity;

    public:
        TQuaternion() noexcept;
        TQuaternion(float InX, float InY, float InZ, float InW);
        TQuaternion(const TVector& V, float Scalar);
        explicit TQuaternion(const TVector4& V);
        explicit TQuaternion(_In_reads_(4) const float* pArray);
        TQuaternion(XMFLOAT4 V);
        TQuaternion(const XMFLOAT4& InQuaternion);
        explicit TQuaternion(const DirectX::XMVECTORF32& F);

        TQuaternion(const TQuaternion&)            = default;
        TQuaternion& operator=(const TQuaternion&) = default;

        TQuaternion(TQuaternion&&)            = default;
        TQuaternion& operator=(TQuaternion&&) = default;

        operator DirectX::XMVECTOR() const { return XMLoadFloat4(this); }

        // Comparison operators
        bool operator ==(const TQuaternion& q) const;
        bool operator !=(const TQuaternion& q) const;

        // Assignment operators
        TQuaternion& operator=(const DirectX::XMVECTORF32& F);

        TQuaternion& operator+=(const TQuaternion& q);
        TQuaternion& operator-=(const TQuaternion& q);
        TQuaternion& operator*=(const TQuaternion& q);
        TQuaternion& operator*=(float S);
        TQuaternion& operator/=(const TQuaternion& q);

        // Unary operators
        TQuaternion operator+() const { return *this; }
        TQuaternion operator-() const;

        // TQuaternion operations
        float Length() const;
        float LengthSquared() const;

        void Normalize();
        void Normalize(TQuaternion& result) const;

        void Conjugate();
        void Conjugate(TQuaternion& result) const;

        void Inverse(TQuaternion& result) const;

        float Dot(const TQuaternion& Q) const;
    };
}
