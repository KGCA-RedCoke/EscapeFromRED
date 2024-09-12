#include "TMatrix.h"
#include "Vector4.h"
/*
  XMMATRIX : 새로운 행렬을 만들 때 사용 (SIMD 연산 가능)
  XMMatrixSet : 행렬을 만들고 값을 초기화할 때 사용
  XMMatrixIdentity : 단위행렬 생성
  XMFLOAT4X4 : 4*4구조체를 이용해 행렬의 원소들을 접근하면서 사용 가능
  XMMatrixMultiply : 행렬을 곱함
  XMMatrixInverse : 역행렬을 만듬
  XMMatrixTranspose : 전치행렬로 바꿈 (4,1 원소가 1,4가 되게 하는게 전치행렬)
*/

const JMath::TVector JMath::TVector::ZeroVector(0, 0, 0);
const JMath::TVector JMath::TVector::OneVector(1, 1, 1);
const JMath::TVector JMath::TVector::UpVector(0, 0, 1);
const JMath::TVector JMath::TVector::DownVector(0, 0, -1);
const JMath::TVector JMath::TVector::ForwardVector(1, 0, 0);
const JMath::TVector JMath::TVector::BackwardVector(-1, 0, 0);
const JMath::TVector JMath::TVector::RightVector(0, 1, 0);
const JMath::TVector JMath::TVector::LeftVector(0, -1, 0);
const JMath::TVector JMath::TVector::XAxisVector(1, 0, 0);
const JMath::TVector JMath::TVector::YAxisVector(0, 1, 0);
const JMath::TVector JMath::TVector::ZAxisVector(0, 0, 1);

const JMath::TVector4 JMath::TVector4::ZeroVector(0, 0, 0, 0);
const JMath::TVector4 JMath::TVector4::OneVector(1, 1, 1, 1);

const JMath::TVector2 JMath::TVector2::ZeroVector(0.f, 0.f);
const JMath::TVector2 JMath::TVector2::UnitVector(1.f, 1.f);
const JMath::TVector2 JMath::TVector2::Unit45Deg(M_INVERSE_SQRT_2, M_INVERSE_SQRT_2);

const JMath::TMatrix JMath::TMatrix::Identity{
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
};
const JMath::TQuaternion JMath::TQuaternion::Identity(0, 0, 0, 1);
