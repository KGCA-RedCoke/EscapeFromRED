#include "CollisionUtils.h"

namespace Utils::Collision
{
	/**
	 * 
	 * @param RayOrigin 
	 * @param RayDirection 
	 * @param BoxMin 
	 * @param BoxMax 
	 * @param OutDistance 
	 * @return 
	 */
	bool RayIntersectsAABB(const FVector& RayOrigin, const FVector& RayDirection, const FVector& BoxMin,
						   const FVector& BoxMax, float&            OutDistance)
	{
		// 역수 계산 ( 1 / RayDirection ) 나눗셈 비용 대신 곱셈을 사용하여 계산 
		XMVECTOR invDir = XMVectorReciprocal(RayDirection);

		XMVECTOR tMin = (BoxMin - RayOrigin) * invDir;
		XMVECTOR tMax = (BoxMax - RayOrigin) * invDir;

		XMVECTOR t1 = XMVectorMin(tMin, tMax);
		XMVECTOR t2 = XMVectorMax(tMin, tMax);


		float tNear = FMath::Max(FMath::Max(XMVectorGetX(t1), XMVectorGetY(t1)), XMVectorGetZ(t1));
		float tFar  = FMath::Min(FMath::Min(XMVectorGetX(t2), XMVectorGetY(t2)), XMVectorGetZ(t2));

		OutDistance = tNear;

		return (tNear <= tFar) && (tFar > 0.0f);
	}
}
