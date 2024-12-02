#pragma once
#include <DirectXMath.h>

#include "Quaternion.h"
#include "Vector.h"

namespace JMath
{
    using namespace DirectX;

    struct TMatrix : public XMFLOAT4X4
    {
        // Constants
        static const TMatrix Identity;

    public:
        TMatrix() noexcept;

        TMatrix(float m00, float m01, float m02, float m03,
                          float m10, float m11, float m12, float m13,
                          float m20, float m21, float m22, float m23,
                          float m30, float m31, float m32, float m33);

        explicit TMatrix(const TVector& r0, const TVector& r1, const TVector& r2);
        explicit TMatrix(const TVector4 r0, const TVector4& r1, const TVector4& r2, const TVector4& r3);

        TMatrix(const XMFLOAT4X4& M);
        TMatrix(const XMFLOAT3X3& M);
        TMatrix(const XMFLOAT4X3& M);

        explicit TMatrix(_In_reads_(16) const float* pArray);
        TMatrix(CXMMATRIX M);

        TMatrix(const TMatrix&)            = default;
        TMatrix& operator=(const TMatrix&) = default;

        TMatrix(TMatrix&&)            = default;
        TMatrix& operator=(TMatrix&&) = default;

        operator XMFLOAT4X4() const { return *this; }

        operator DirectX::XMMATRIX() const { return XMLoadFloat4x4(this); }

        // Comparison operators
        bool operator ==(const TMatrix& M) const;
        bool operator !=(const TMatrix& M) const;

        // Assignment operators
        TMatrix& operator=(const XMFLOAT3X3& M);
        TMatrix& operator=(const XMFLOAT4X3& M);
        TMatrix& operator+=(const TMatrix& M);
        TMatrix& operator-=(const TMatrix& M);
        TMatrix& operator*=(const TMatrix& M);
        TMatrix& operator*=(float S);
        TMatrix& operator/=(float S);

        TMatrix& operator/=(const TMatrix& M);
        // Element-wise divide

        // Unary operators
        TMatrix operator+() const { return *this; }
        TMatrix operator-() const;
        TMatrix operator*(const TMatrix& M);
        TMatrix operator*(TMatrix& M);
        TMatrix operator*(float Scale);

        // Properties
        TVector Up() const;

        void Up(const TVector& v);

        TVector Down() const;

        void Down(const TVector& v);

        TVector Right() const;

        void Right(const TVector& v);

        TVector Left() const;

        void Left(const TVector& v);

        TVector Forward() const;

        void Forward(const TVector& v);

        TVector Backward() const;

        void Backward(const TVector& v);

        TVector Translation() const;

        void Translation(const TVector& v);

        // TMatrix operations
        bool Decompose(TVector& scale, TQuaternion& rotation, TVector& translation);

        TMatrix Transpose() const;
        void    Transpose(TMatrix& result) const;

        TMatrix Invert() const;
        void    Invert(TMatrix& result) const;

        float Determinant() const;

        static TMatrix* D3DXMatrixTranspose(TMatrix* pOut, CONST TMatrix* pM)
        {
            *pOut = pM->Transpose();
            return pOut;
        }
    };
}
